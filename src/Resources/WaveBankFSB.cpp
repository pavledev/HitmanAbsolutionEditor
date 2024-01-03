#include <format>

#include "Resources/WaveBankFSB.h"
#include "FSB/FSB.h"
#include "FSB/Container.h"

WaveBankFSB::AudioSample::~AudioSample()
{
	operator delete(data);
}

void WaveBankFSB::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else if (exportOption.starts_with("FSB"))
	{
		void* fsb5Data = nullptr;
		unsigned int fsb5Size = 0;

		GetFSB5Data(fsb5Data, fsb5Size);

		BinaryWriter binaryWriter = BinaryWriter(outputPath);

		binaryWriter.Write(fsb5Data, fsb5Size);
	}
	else
	{
		const std::string extension = exportOption.substr(exportOption.find("(") + 1, 3);

		if (extension == "OGG")
		{
			ConvertFSB5ToOGG();
		}
		else
		{
			ConvertFSB5ToWAV();
		}

		for (size_t i = 0; i < audioSamples.size(); ++i)
		{
			const std::string outputFilePath = std::format("{}\\{}.{}", outputPath, audioSamples[i]->name, extension);
			BinaryWriter binaryWriter = BinaryWriter(outputFilePath);

			binaryWriter.Write(audioSamples[i]->data, audioSamples[i]->dataSize);
		}
	}
}

const std::vector<std::shared_ptr<WaveBankFSB::AudioSample>>& WaveBankFSB::GetAudioSamples() const
{
	return audioSamples;
}

std::vector<std::shared_ptr<WaveBankFSB::AudioSample>>& WaveBankFSB::GetAudioSamples()
{
	return audioSamples;
}

void WaveBankFSB::ConvertFSB5ToOGG()
{
	void* fsb5Data = nullptr;
	unsigned int fsb5Size = 0;

	GetFSB5Data(fsb5Data, fsb5Size);

	BinaryReader binaryReader = BinaryReader(fsb5Data, fsb5Size);
	FSB::Container container = FSB::Container(binaryReader);

	audioSamples.reserve(container.Samples().size());

	for (const FSB::Sample& sample : container.Samples())
	{
		void* data = operator new(sample.size);
		BinaryWriter binaryWriter = BinaryWriter(data, sample.size);

		container.ExtractSample(sample, binaryWriter);

		std::shared_ptr<AudioSample> audioSample = std::make_shared<AudioSample>();

		audioSample->name = sample.name;
		audioSample->data = binaryWriter.GetBuffer();
		audioSample->dataSize = static_cast<unsigned int>(binaryWriter.GetPosition());

		audioSamples.push_back(audioSample);
	}
}

void WaveBankFSB::ConvertFSB5ToWAV()
{
	void* fsb5Data = nullptr;
	unsigned int fsb5Size = 0;

	GetFSB5Data(fsb5Data, fsb5Size);

	BinaryReader binaryReader = BinaryReader(fsb5Data, fsb5Size);
	FSB::Container container = FSB::Container(binaryReader);

	audioSamples.reserve(container.Samples().size());

	for (const FSB::Sample& sample : container.Samples())
	{
		void* data = operator new(sample.size);
		BinaryWriter binaryWriter = BinaryWriter(data, sample.size);

		container.ExtractSample(sample, binaryWriter);

		std::shared_ptr<AudioSample> audioSample = std::make_shared<AudioSample>();

		audioSample->name = sample.name;
		audioSample->data = binaryWriter.GetBuffer();
		audioSample->dataSize = static_cast<unsigned int>(binaryWriter.GetPosition());

		audioSamples.push_back(audioSample);
	}
}
