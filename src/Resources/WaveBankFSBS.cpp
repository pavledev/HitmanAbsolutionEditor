#include <format>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Resources/WaveBankFSBS.h"
#include "Utility/StringUtility.h"
#include "Utility/ResourceUtility.h"
#include "Logger.h"
#include "FSB/Container.h"
#include "Registry/ResourceInfoRegistry.h"

FSB::Format WaveBankFSBS::GetFormat()
{
	void* fsb5Data = nullptr;
	unsigned int fsb5Size = 0;

	GetFSB5Data(fsb5Data, fsb5Size);

	BinaryReader binaryReader = BinaryReader(fsb5Data, fsb5Size);
	unsigned int offset = offsetof(FSB::Header, mode);

	binaryReader.Seek(offset);

	return static_cast<FSB::Format>(binaryReader.Read<unsigned int>());
}

void WaveBankFSBS::GetFSB5Data(void*& fsb5Data, unsigned int& fsb5Size)
{
	const unsigned int fsb5DataOffset = FindFSB5DataOffsetInFSBS();
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

	binaryReader.Seek(fsb5DataOffset);

	fsb5Data = binaryReader.GetBuffer(true);
	fsb5Size = static_cast<unsigned int>(binaryReader.GetSize()) - fsb5DataOffset;
}

void WaveBankFSBS::GetFilePaths(std::vector<std::string>& filePaths)
{
	char* resourceData = static_cast<char*>(GetResourceData());
	const unsigned int resourceDataSize = GetResourceDataSize();
	size_t position = 0;

	while (true)
	{
		size_t index = StringUtility::FindString(static_cast<char*>(resourceData), R"(Assembly\Sound\Localization\Dialogue)", resourceDataSize, position);

		if (index == std::string::npos)
		{
			break;
		}

		std::string releaseFilePath;

		while (resourceData[index] != '\0')
		{
			releaseFilePath += resourceData[index];

			++index;
		}

		position = index;

		filePaths.push_back(releaseFilePath);
	}
}

unsigned int WaveBankFSBS::FindFSB5DataOffsetInFSBS()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

	binaryReader.Seek(24 + 16);

	unsigned int magic = binaryReader.Read<unsigned int>();
	unsigned int offset = -1;

	if (magic == '5BSF')
	{
		offset = static_cast<unsigned int>(binaryReader.GetPosition()) - 4;
	}
	else
	{
		binaryReader.Seek(36);

		magic = binaryReader.Read<unsigned int>();

		if (magic == 'ECAF')
		{
			binaryReader.Seek(24);

			const unsigned int size = binaryReader.Read<unsigned int>();

			binaryReader.Seek(binaryReader.GetPosition() - 4 + size);

			while (true)
			{
				if (binaryReader.Read<unsigned char>() == 'F')
				{
					binaryReader.Seek(binaryReader.GetPosition() - 1);

					magic = binaryReader.Read<unsigned int>();

					if (magic == '5BSF')
					{
						break;
					}
				}
			}

			offset = static_cast<unsigned int>(binaryReader.GetPosition()) - 4;
		}
	}

	return offset;
}

bool WaveBankFSBS::PatchResourceLibrary(const std::string& oggFilePath, int indexToReplace)
{
	if (resourceLibraryFilePath.empty())
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Resource library file path is empty");

		return false;
	}

	if (!std::filesystem::exists(resourceLibraryFilePath))
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Resource library file not found: {}", resourceLibraryFilePath));

		return false;
	}

	if (headerLibraryFilePath.empty())
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Header library file path is empty");

		return false;
	}

	if (!std::filesystem::exists(headerLibraryFilePath))
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Header library file not found: {}", headerLibraryFilePath));

		return false;
	}

	std::string backupResLibPath = resourceLibraryFilePath + ".bak";
	std::string backupHdrLibPath = headerLibraryFilePath + ".bak";

	if (!std::filesystem::exists(backupResLibPath))
	{
		std::filesystem::copy_file(resourceLibraryFilePath, backupResLibPath, std::filesystem::copy_options::overwrite_existing);
		Logger::GetInstance().Log(Logger::Level::Info, std::format("Created resource library backup: {}", backupResLibPath));
	}

	if (!std::filesystem::exists(backupHdrLibPath))
	{
		std::filesystem::copy_file(headerLibraryFilePath, backupHdrLibPath, std::filesystem::copy_options::overwrite_existing);
		Logger::GetInstance().Log(Logger::Level::Info, std::format("Created header library backup: {}", backupHdrLibPath));
	}

	std::string fsbankclPath;

	char exePathBuf[MAX_PATH];
	GetModuleFileNameA(nullptr, exePathBuf, MAX_PATH);
	std::string exeDir = std::filesystem::path(exePathBuf).parent_path().string();

	std::string pathsToTry[] = {
		exeDir + "/Tools/fsbankcl.exe",
		exeDir + "/../Tools/fsbankcl.exe",
		exeDir + "/../../Tools/fsbankcl.exe",
		"Tools/fsbankcl.exe",
		"fsbankcl.exe"
	};

	for (const auto& path : pathsToTry)
	{
		if (std::filesystem::exists(path))
		{
			fsbankclPath = path;

			break;
		}
	}

	if (fsbankclPath.empty())
	{
		Logger::GetInstance().Log(Logger::Level::Error, "fsbankcl.exe not found. Place it in Tools/ directory or system PATH.");

		return false;
	}

	std::string patchDir = exeDir + "\\fsbs_patch";

	if (std::filesystem::exists(patchDir))
	{
		std::filesystem::remove_all(patchDir);
	}

	std::filesystem::create_directories(patchDir);

	std::string fsbOutputPath = patchDir + "\\output.fsb";
	std::string lstFilePath = patchDir + "\\files.lst";
	auto& samples = GetAudioSamples();

	if (samples.empty())
	{
		ConvertFSB5ToWAV(); // Ensure samples are loaded and converted
	}

	std::ofstream lstFile(lstFilePath);

	if (samples.size() <= 1)
	{
		lstFile << oggFilePath << "\n";
	}
	else
	{
		// Extract existing samples and substitute the one being replaced
		for (size_t i = 0; i < samples.size(); ++i)
		{
			if (static_cast<int>(i) == indexToReplace)
			{
				lstFile << oggFilePath << "\n";
			}
			else
			{
				std::string tempWavPath = std::format("{}\\temp_{}.wav", patchDir, i);
				BinaryWriter bw(tempWavPath);
				bw.Write(samples[i]->data, samples[i]->dataSize);
				lstFile << tempWavPath << "\n";
			}
		}
	}

	lstFile.close();

	std::string command = std::format("\"{}\" -format vorbis -o \"{}\" \"{}\"", fsbankclPath, fsbOutputPath, lstFilePath);

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Running: {}", command));

	// Use CreateProcessA instead of std::system to avoid cmd.exe quote-stripping issues.
	// When std::system passes a string starting with '"' to cmd /c with multiple quoted args,
	// cmd.exe strips the first and last quote, breaking the command entirely.
	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	HANDLE hReadPipe = nullptr;
	HANDLE hWritePipe = nullptr;
	CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFOA si{};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.hStdInput = nullptr;

	PROCESS_INFORMATION pi{};

	std::string mutableCommand = command;

	BOOL created = CreateProcessA(
		nullptr,
		mutableCommand.data(),
		nullptr,
		nullptr,
		TRUE,
		CREATE_NO_WINDOW,
		nullptr,
		nullptr,
		&si,
		&pi
	);

	if (!created)
	{
		DWORD err = GetLastError();
		CloseHandle(hReadPipe);
		CloseHandle(hWritePipe);
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to launch fsbankcl.exe (Win32 error {})", err));

		std::filesystem::remove_all(patchDir);

		return false;
	}

	CloseHandle(hWritePipe);

	// Read all output from fsbankcl
	std::string processOutput;
	char buf[4096];
	DWORD bytesRead = 0;

	while (ReadFile(hReadPipe, buf, sizeof(buf) - 1, &bytesRead, nullptr) && bytesRead > 0)
	{
		buf[bytesRead] = '\0';
		processOutput += buf;
	}

	CloseHandle(hReadPipe);

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitCode = 0;
	GetExitCodeProcess(pi.hProcess, &exitCode);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	int result = static_cast<int>(exitCode);

	if (result != 0 || !std::filesystem::exists(fsbOutputPath))
	{
		if (!processOutput.empty())
		{
			Logger::GetInstance().Log(Logger::Level::Error, std::format("fsbankcl.exe output:\n{}", processOutput));
		}

		Logger::GetInstance().Log(Logger::Level::Error, std::format("fsbankcl.exe failed with exit code {}", result));

		std::filesystem::remove_all(patchDir);

		return false;
	}


	std::ifstream fsbFile(fsbOutputPath, std::ios::binary | std::ios::ate);

	if (!fsbFile.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to open FSB file: {}", fsbOutputPath));

		std::filesystem::remove_all(patchDir);

		return false;
	}

	std::streamsize fsbSize = fsbFile.tellg();

	fsbFile.seekg(0, std::ios::beg);

	std::vector<unsigned char> newFsbData(fsbSize);
	fsbFile.read(reinterpret_cast<char*>(newFsbData.data()), fsbSize);
	fsbFile.close();

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Read FSB5 data ({} bytes)", newFsbData.size()));

	unsigned int fsb5Offset = FindFSB5DataOffsetInFSBS();

	std::vector<unsigned char> newResourceData;
	auto* resData = static_cast<unsigned char*>(resourceData);
	newResourceData.insert(newResourceData.end(), resData, resData + fsb5Offset);
	newResourceData.insert(newResourceData.end(), newFsbData.begin(), newFsbData.end());

	// Update size fields in the FSBS prefix header.
	// The prefix (bytes before FSB5) may contain uint32 size fields that the game engine
	// uses to know how much FSB5 data to stream. If we don't update them, the game will
	// only read the old amount of data, cutting off the new (longer) audio.
	unsigned int oldFsb5Size = resourceDataSize - fsb5Offset;
	unsigned int newFsb5Size = static_cast<unsigned int>(newFsbData.size());
	unsigned int oldDataSizeInHeader = resourceDataSize - 24;
	unsigned int newDataSizeInHeader = static_cast<unsigned int>(newResourceData.size()) - 24;

	// Log prefix bytes for diagnostics
	std::string prefixHex;
	for (unsigned int i = 0; i < fsb5Offset && i < static_cast<unsigned int>(newResourceData.size()); i += 4)
	{
		unsigned int val;
		memcpy(&val, &newResourceData[i], sizeof(unsigned int));
		prefixHex += std::format("[{}]=0x{:08X} ", i, val);
	}
	Logger::GetInstance().Log(Logger::Level::Info, std::format("FSBS prefix ({} bytes): {}", fsb5Offset, prefixHex));
	Logger::GetInstance().Log(Logger::Level::Info, std::format("Old FSB5 size: {}, New FSB5 size: {}, Old m_nDataSize: {}, New m_nDataSize: {}",
		oldFsb5Size, newFsb5Size, oldDataSizeInHeader, newDataSizeInHeader));

	// Scan prefix for uint32 values matching old sizes and update to new sizes
	for (unsigned int i = 0; i + 4 <= fsb5Offset; i += 4)
	{
		unsigned int val;
		memcpy(&val, &newResourceData[i], sizeof(unsigned int));

		if (val == oldFsb5Size)
		{
			Logger::GetInstance().Log(Logger::Level::Info, std::format("Updating FSBS prefix at offset {}: {} -> {} (old FSB5 size)", i, oldFsb5Size, newFsb5Size));
			memcpy(&newResourceData[i], &newFsb5Size, sizeof(unsigned int));
		}
		else if (val == resourceDataSize)
		{
			unsigned int newTotalResSize = static_cast<unsigned int>(newResourceData.size());
			Logger::GetInstance().Log(Logger::Level::Info, std::format("Updating FSBS prefix at offset {}: {} -> {} (old total resource size)", i, resourceDataSize, newTotalResSize));
			memcpy(&newResourceData[i], &newTotalResSize, sizeof(unsigned int));
		}
		else if (val == oldDataSizeInHeader)
		{
			Logger::GetInstance().Log(Logger::Level::Info, std::format("Updating FSBS prefix at offset {}: {} -> {} (old m_nDataSize)", i, oldDataSizeInHeader, newDataSizeInHeader));
			memcpy(&newResourceData[i], &newDataSizeInHeader, sizeof(unsigned int));
		}
	}

	BinaryReader headerLibReader = BinaryReader(headerLibraryFilePath);

	headerLibReader.Seek(4, SeekOrigin::Begin);

	unsigned int referencesChunkSize = headerLibReader.Read<unsigned int>();

	headerLibReader.Seek(referencesChunkSize + 24 + 16, SeekOrigin::Begin);

	unsigned int chunksStartOffset = headerLibReader.GetPosition();
	chunksStartOffset += headerLibReader.Read<unsigned int>();

	headerLibReader.Seek(chunksStartOffset - 4, SeekOrigin::Begin);

	unsigned int chunkCount = headerLibReader.Read<unsigned int>();

	unsigned int targetChunkResourceHeadersStartOffset = 0;
	unsigned int targetChunkResourceHeaderCount = 0;

	for (unsigned int c = 0; c < chunkCount; ++c)
	{
		unsigned int chunkOffset = chunksStartOffset + 0x50 * c;

		headerLibReader.Seek(chunkOffset + 0x4, SeekOrigin::Begin);

		unsigned int headerLibraryChunkResourceIDOffset = headerLibReader.GetPosition();
		headerLibraryChunkResourceIDOffset += headerLibReader.Read<unsigned int>();

		headerLibReader.Seek(headerLibraryChunkResourceIDOffset - 4, SeekOrigin::Begin);

		unsigned int resourceIDLength = headerLibReader.Read<unsigned int>();
		std::string chunkResourceID = headerLibReader.ReadString(static_cast<size_t>(resourceIDLength - 1));

		if (chunkResourceID != headerLibraryChunkResourceID)
		{
			continue;
		}

		headerLibReader.Seek(chunkOffset + 0x2C, SeekOrigin::Begin);

		unsigned int resourceHeadersStartOffset = headerLibReader.GetPosition();
		resourceHeadersStartOffset += headerLibReader.Read<unsigned int>();

		headerLibReader.Seek(resourceHeadersStartOffset - 4, SeekOrigin::Begin);

		unsigned int resourceHeaderCount = headerLibReader.Read<unsigned int>();

		targetChunkResourceHeadersStartOffset = resourceHeadersStartOffset;
		targetChunkResourceHeaderCount = resourceHeaderCount;

		break;
	}

	if (targetChunkResourceHeadersStartOffset == 0)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Could not find target chunk in header library");

		std::filesystem::remove_all(patchDir);

		return false;
	}

	std::vector<std::pair<unsigned int, unsigned int>> resourceSizes;

	unsigned int resourceLibraryOffset = 0x18;

	headerLibReader.Seek(targetChunkResourceHeadersStartOffset, SeekOrigin::Begin);

	for (unsigned int i = 0; i < targetChunkResourceHeaderCount; ++i)
	{
		unsigned int currentPosition = headerLibReader.GetPosition();
		unsigned int relativeOffset = headerLibReader.Read<unsigned int>();
		unsigned int absoluteOffset = currentPosition + relativeOffset;

		headerLibReader.Seek(absoluteOffset, SeekOrigin::Begin);

		unsigned int type = headerLibReader.Read<unsigned int>();

		headerLibReader.Skip(8);

		unsigned int dataSize = headerLibReader.Read<unsigned int>();

		if (type == 'FSBM' || type == 'FSBS')
		{
			dataSize += 24;
		}

		resourceSizes.push_back({ absoluteOffset + 12, dataSize });
		resourceLibraryOffset += dataSize;

		headerLibReader.Seek(currentPosition + 0xC, SeekOrigin::Begin);
	}

	std::ifstream resLibFile(resourceLibraryFilePath, std::ios::binary | std::ios::ate);

	if (!resLibFile.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to open resource library for reading: {}", resourceLibraryFilePath));

		std::filesystem::remove_all(patchDir);

		return false;
	}

	std::streamsize totalSize = resLibFile.tellg();

	resLibFile.seekg(0, std::ios::beg);

	std::vector<unsigned char> resLibData(totalSize);
	resLibFile.read(reinterpret_cast<char*>(resLibData.data()), totalSize);
	resLibFile.close();

	unsigned int chunkStartOffset = 0x18;

	for (unsigned int i = 0; i < indexInLibrary; ++i)
	{
		chunkStartOffset += resourceSizes[i].second;
	}

	std::vector<unsigned char> newResLibData;
	newResLibData.insert(newResLibData.end(), resLibData.begin(), resLibData.begin() + chunkStartOffset);
	newResLibData.insert(newResLibData.end(), newResourceData.begin(), newResourceData.end());
	newResLibData.insert(newResLibData.end(), resLibData.begin() + chunkStartOffset + resourceDataSize, resLibData.end());

	unsigned int newTotalDataSize = static_cast<unsigned int>(newResLibData.size() - 0x18);
	newResLibData[0xC] = newTotalDataSize & 0xFF;
	newResLibData[0xD] = (newTotalDataSize >> 8) & 0xFF;
	newResLibData[0xE] = (newTotalDataSize >> 16) & 0xFF;
	newResLibData[0xF] = (newTotalDataSize >> 24) & 0xFF;

	std::ofstream resLibOutFile(resourceLibraryFilePath, std::ios::binary | std::ios::trunc);

	if (!resLibOutFile.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to open resource library for writing: {}", resourceLibraryFilePath));

		std::filesystem::remove_all(patchDir);

		return false;
	}

	resLibOutFile.write(reinterpret_cast<const char*>(newResLibData.data()), newResLibData.size());
	resLibOutFile.close();

	unsigned int dataSizeOffset = resourceSizes[indexInLibrary].first;

	std::fstream headerLibFile(headerLibraryFilePath, std::ios::binary | std::ios::in | std::ios::out);

	if (!headerLibFile.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to open header library for writing: {}", headerLibraryFilePath));

		std::filesystem::remove_all(patchDir);

		return false;
	}

	unsigned int newDataSetSize = static_cast<unsigned int>(newResourceData.size()) - 24;
	headerLibFile.seekp(dataSizeOffset);
	headerLibFile.write(reinterpret_cast<const char*>(&newDataSetSize), sizeof(newDataSetSize));
	headerLibFile.close();

	delete[] static_cast<unsigned char*>(resourceData);

	resourceDataSize = static_cast<unsigned int>(newResourceData.size());
	resourceData = new unsigned char[resourceDataSize];
	memcpy(resourceData, newResourceData.data(), resourceDataSize);

	std::filesystem::remove_all(patchDir);

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Patched resource {} in {} (old size: {}, new size: {})", resourceID, resourceLibraryFilePath, resourceDataSize, newResourceData.size()));

	// Update the paired WAVB resource's m_fDuration to match the new audio length.
	// The game uses m_fDuration from WAVB to decide when to stop playback.
	PatchWAVBDuration(newFsbData);

	return true;
}

void WaveBankFSBS::PatchWAVBDuration(const std::vector<unsigned char>& newFsbData)
{
	// Parse the new FSB5 to get sample count and frequency
	float newDuration = 0.0f;

	try
	{
		BinaryReader fsbReader(const_cast<unsigned char*>(newFsbData.data()), static_cast<unsigned int>(newFsbData.size()));
		FSB::Container container(fsbReader);

		const auto& samples = container.Samples();

		if (samples.empty())
		{
			Logger::GetInstance().Log(Logger::Level::Warning, "New FSB5 has no samples, skipping WAVB duration update");
			return;
		}

		// Calculate total duration from all samples (usually just one)
		for (const auto& sample : samples)
		{
			if (sample.frequency > 0)
			{
				float sampleDuration = static_cast<float>(sample.samples) / static_cast<float>(sample.frequency);

				if (sampleDuration > newDuration)
				{
					newDuration = sampleDuration;
				}
			}
		}

		Logger::GetInstance().Log(Logger::Level::Info, std::format("New audio duration: {:.3f} seconds", newDuration));
	}
	catch (const std::exception& ex)
	{
		Logger::GetInstance().Log(Logger::Level::Warning, std::format("Failed to parse new FSB5 for duration: {}", ex.what()));
		return;
	}

	if (newDuration <= 0.0f)
	{
		Logger::GetInstance().Log(Logger::Level::Warning, "Computed duration is 0, skipping WAVB update");
		return;
	}

	// Find WAVB resource among back-references
	const auto& backRefs = GetBackReferences();
	std::shared_ptr<Resource> wavbResource;

	for (const auto& backRef : backRefs)
	{
		const ResourceInfoRegistry::ResourceInfo& refInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(backRef->GetHash());

		if (refInfo.type == "WAVB")
		{
			wavbResource = backRef;
			break;
		}
	}

	if (!wavbResource)
	{
		// Also check forward references
		const auto& refs = GetReferences();

		for (const auto& ref : refs)
		{
			const ResourceInfoRegistry::ResourceInfo& refInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(ref->GetHash());

			if (refInfo.type == "WAVB")
			{
				wavbResource = ref;
				break;
			}
		}
	}

	if (!wavbResource)
	{
		Logger::GetInstance().Log(Logger::Level::Warning, "No WAVB back-reference found, skipping duration update");
		return;
	}

	const ResourceInfoRegistry::ResourceInfo& wavbInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(wavbResource->GetHash());

	if (wavbInfo.headerLibraries.empty())
	{
		Logger::GetInstance().Log(Logger::Level::Warning, "WAVB has no header libraries, skipping duration update");
		return;
	}

	// Load WAVB resource to find its position in the resource library
	std::shared_ptr<WaveBank> wavbRes = std::make_shared<WaveBank>();
	wavbRes->SetHash(wavbInfo.hash);
	wavbRes->SetResourceID(wavbInfo.resourceID);
	wavbRes->SetHeaderLibraries(&wavbInfo.headerLibraries);
	wavbRes->LoadResource(0, wavbInfo.headerLibraries[0].chunkIndex, wavbInfo.headerLibraries[0].indexInLibrary, false, false, true);

	if (!wavbRes->IsResourceLoaded() || !wavbRes->GetResourceData())
	{
		Logger::GetInstance().Log(Logger::Level::Warning, "Failed to load WAVB resource data, skipping duration update");
		return;
	}

	// m_fDuration is at offset 20 in SWaveBankResourceData
	// struct layout: m_nTracks(4) + m_eLoadType(4) + m_eFormat(4) + m_nIsPlaceholder(4) + m_nIsNotDucking(4) + m_fDuration(4)
	const unsigned int durationFieldOffset = 20;

	// Read old duration for logging
	float oldDuration = 0.0f;
	memcpy(&oldDuration, static_cast<unsigned char*>(wavbRes->GetResourceData()) + durationFieldOffset, sizeof(float));

	// Write new duration to the WAVB resource in the resource library file on disk
	std::string wavbResourceLibPath = wavbRes->GetResourceLibraryFilePath();
	unsigned int wavbOffsetInResLib = wavbRes->GetOffsetInResourceLibrary();

	std::fstream wavbFile(wavbResourceLibPath, std::ios::binary | std::ios::in | std::ios::out);

	if (!wavbFile.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Warning, std::format("Failed to open WAVB resource library for writing: {}", wavbResourceLibPath));
		return;
	}

	wavbFile.seekp(wavbOffsetInResLib + durationFieldOffset);
	wavbFile.write(reinterpret_cast<const char*>(&newDuration), sizeof(float));
	wavbFile.close();

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Updated WAVB m_fDuration: {:.3f} -> {:.3f} seconds ({})", oldDuration, newDuration, wavbInfo.resourceID));
}
