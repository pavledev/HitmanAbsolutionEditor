#include <sstream>

#include "Glacier/Serializer/ZBinaryDeserializer.h"
#include "Registry/TypeRegistry.h"
#include "Logger.h"

ZBinaryDeserializer::ZBinaryDeserializer()
{
	alignment = 0;
}

void* ZBinaryDeserializer::Deserialize(const std::string& filePath, const std::vector<std::shared_ptr<Resource>>* references)
{
	BinaryReader binaryReader = BinaryReader(filePath);

	return Deserialize(binaryReader, references);
}

void* ZBinaryDeserializer::Deserialize(void* buffer, const unsigned int size, const std::vector<std::shared_ptr<Resource>>* references)
{
	BinaryReader binaryReader = BinaryReader(buffer, size);

	return Deserialize(binaryReader, references);
}

void* ZBinaryDeserializer::Deserialize(BinaryReader& binaryReader, const std::vector<std::shared_ptr<Resource>>* references)
{
	unsigned int magic = binaryReader.Read<unsigned int>();

	if (magic != '2NIB')
	{
		Logger::GetInstance().Log(Logger::Level::Error, "File format not supported!");

		return nullptr;
	}

	BinaryBulkDataHeader::EDataEndianness dataEndianness = static_cast<BinaryBulkDataHeader::EDataEndianness>(binaryReader.Read<unsigned char>());
	const unsigned char alignment = binaryReader.Read<unsigned char>();
	const unsigned char sectionsCount = binaryReader.Read<unsigned char>();
	const unsigned char unusedByte = binaryReader.Read<unsigned char>();

	const unsigned char dataLength0 = binaryReader.Read<unsigned char>();
	const unsigned char dataLength1 = binaryReader.Read<unsigned char>();
	const unsigned char dataLength2 = binaryReader.Read<unsigned char>();
	const unsigned char dataLength3 = binaryReader.Read<unsigned char>();
	const unsigned int dataLength = (dataLength0 << 24) + (dataLength1 << 16) + (dataLength2 << 8) + dataLength3;

	unsigned int unusedDWORD = binaryReader.Read<unsigned int>();

	void* data = operator new(dataLength, std::align_val_t(alignment));

	binaryReader.Read(data, dataLength);

	BinaryReader dataSectionBinaryReader(data, dataLength);
	BinaryWriter dataSectionBinaryWriter(data, dataLength);

	for (unsigned char i = 0; i < sectionsCount; ++i)
	{
		const unsigned int sectionType = binaryReader.Read<unsigned int>();
		const unsigned int sectionSize = binaryReader.Read<unsigned int>();

		switch (sectionType)
		{
			case 0x12EBA5ED:
				ParseRebaseSection(binaryReader, dataSectionBinaryReader, dataSectionBinaryWriter);
				break;
			case 0x3989BF9F:
				ParseTypeReindexingSection(binaryReader, dataSectionBinaryReader, dataSectionBinaryWriter);
				break;
			case 0x578FBCEE:
				ParseRuntimeResourceIDReindexingSection(binaryReader, dataSectionBinaryReader, dataSectionBinaryWriter, references);
				break;
			default:
			{
				std::stringstream stream;

				stream << std::hex << sectionType;

				Logger::GetInstance().Log(Logger::Level::Error, "Unknown section type: 0x{}!", stream.str());

				return nullptr;
			}
		}
	}

	this->alignment = alignment;

	return data;
}

const unsigned char ZBinaryDeserializer::GetAlignment() const
{
	return alignment;
}

void ZBinaryDeserializer::ParseRebaseSection(BinaryReader& binaryReader, BinaryReader& dataSectionbinaryReader, BinaryWriter& dataSectionBinaryWriter)
{
	const unsigned int numberOfRebaseLocations = binaryReader.Read<unsigned int>();
	unsigned int rebaseLocationOffset = 0;

	for (unsigned int i = 0; i < numberOfRebaseLocations; ++i)
	{
		rebaseLocationOffset += binaryReader.Read<unsigned int>();

		dataSectionbinaryReader.Seek(rebaseLocationOffset, SeekOrigin::Begin);
		dataSectionBinaryWriter.Seek(rebaseLocationOffset, SeekOrigin::Begin);

		int value = dataSectionbinaryReader.Read<unsigned int>() + rebaseLocationOffset;

		if (value != -1)
		{
			dataSectionBinaryWriter.Write(reinterpret_cast<unsigned int>(dataSectionbinaryReader.GetBuffer()) + value);
		}
		else
		{
			dataSectionBinaryWriter.Write(0);
		}
	}
}

void ZBinaryDeserializer::ParseTypeReindexingSection(BinaryReader& binaryReader, BinaryReader& dataSectionbinaryReader, BinaryWriter& dataSectionBinaryWriter)
{
	const unsigned int sectionStartPosition = binaryReader.GetPosition();
	const unsigned int numberOfOffsetsToReindex = binaryReader.Read<unsigned int>();
	std::unordered_map<unsigned int, unsigned int> typeIDsToReindex;
	unsigned int typeIDOffset = 0;

	for (unsigned int i = 0; i < numberOfOffsetsToReindex; ++i)
	{
		typeIDOffset += binaryReader.Read<unsigned int>();

		dataSectionbinaryReader.Seek(typeIDOffset, SeekOrigin::Begin);

		const unsigned int typeIDIndex = dataSectionbinaryReader.Read<unsigned int>();

		typeIDsToReindex.insert(std::make_pair(typeIDOffset, typeIDIndex));
	}

	const unsigned int numberOfTypeNames = binaryReader.Read<unsigned int>();
	std::vector<STypeID*> typeIDs(numberOfTypeNames);

	for (unsigned int i = 0; i < numberOfTypeNames; ++i)
	{
		unsigned int currentPosition = binaryReader.GetPosition() - sectionStartPosition;

		Align(binaryReader, currentPosition, 4);

		const unsigned int typeIDIndex = binaryReader.Read<unsigned int>();
		const int typeSize = binaryReader.Read<int>();
		const unsigned int typeNameLength = binaryReader.Read<unsigned int>();
		const std::string typeName = binaryReader.ReadString(static_cast<size_t>(typeNameLength - 1));
		STypeID* typeID = GetTypeIDFromTypeName(typeName);

		//Create type id for enums
		if (!typeID)
		{
			typeID = new STypeID();

			typeID->flags = 0;
			typeID->typeNum = -1;

			IType* type = new IType(4, 4, 8, typeName.c_str(), typeID);

			typeID->pTypeInfo = type;

			TypeRegistry::GetInstance().RegisterType(typeID);
		}

		typeIDs[typeIDIndex] = typeID;
	}

	for (auto it = typeIDsToReindex.begin(); it != typeIDsToReindex.end(); it++)
	{
		const unsigned int typeIDOffset = it->first;
		const unsigned int typeIDIndex = it->second;

		dataSectionBinaryWriter.Seek(typeIDOffset, SeekOrigin::Begin);
		dataSectionBinaryWriter.Write(reinterpret_cast<unsigned int>(typeIDs[typeIDIndex]));
	}
}

void ZBinaryDeserializer::ParseRuntimeResourceIDReindexingSection(BinaryReader& binaryReader, BinaryReader& dataSectionbinaryReader,
	BinaryWriter& dataSectionBinaryWriter, const std::vector<std::shared_ptr<Resource>>* references)
{
	if (!references)
	{
		return;
	}

	const unsigned int numberOfOffsetsToReindex = binaryReader.Read<unsigned int>();

	for (unsigned int i = 0; i < numberOfOffsetsToReindex; ++i)
	{
		const unsigned int runtimeResourceIDOffset = binaryReader.Read<unsigned int>();

		dataSectionbinaryReader.Seek(runtimeResourceIDOffset, SeekOrigin::Begin);

		const unsigned int idHigh = dataSectionbinaryReader.Read<unsigned int>();
		const unsigned int idLow = dataSectionbinaryReader.Read<unsigned int>(); //Index of header reference

		if (idLow != UINT32_MAX)
		{
			const ZRuntimeResourceID runtimeResourceID = references->operator[](idLow)->GetRuntimeResourceID();

			dataSectionBinaryWriter.Seek(runtimeResourceIDOffset, SeekOrigin::Begin);
			dataSectionBinaryWriter.Write(runtimeResourceID);
		}
	}
}

void ZBinaryDeserializer::Align(BinaryReader& binaryReader, const unsigned int currentPosition, const unsigned char alignment)
{
	if (currentPosition % alignment != 0)
	{
		const unsigned int paddingCount = alignment - currentPosition % alignment;

		binaryReader.Skip(paddingCount);
	}
}

STypeID* ZBinaryDeserializer::GetTypeIDFromTypeName(const std::string& typeName)
{
	return TypeRegistry::GetInstance().GetTypeID(typeName);
}
