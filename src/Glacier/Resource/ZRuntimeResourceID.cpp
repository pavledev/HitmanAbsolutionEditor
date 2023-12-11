#include <format>
#include <Windows.h>

#include "Glacier/Resource/ZRuntimeResourceID.h"
#include "Glacier/Serializer/ZBinarySerializer.h"
#include "Hash.h"
#include "Registry/ResourceIDRegistry.h"
#include "Registry/ResourceInfoRegistry.h"

ZRuntimeResourceID::ZRuntimeResourceID()
{
	m_IDHigh = 0;
	m_IDLow = 0;
}

ZRuntimeResourceID::ZRuntimeResourceID(unsigned long long runtimeResourceID)
{
	m_IDHigh = runtimeResourceID >> 32;
	m_IDLow = static_cast<unsigned int>(runtimeResourceID);
}

ZRuntimeResourceID::ZRuntimeResourceID(unsigned int idHigh, unsigned int idLow)
{
	m_IDHigh = idHigh;
	m_IDLow = idLow;
}

ZRuntimeResourceID::operator long long() const
{
	return GetID();
}

unsigned long long ZRuntimeResourceID::GetID() const
{
	return (static_cast<unsigned long long>(m_IDHigh) << 32) | m_IDLow;
}

ZRuntimeResourceID ZRuntimeResourceID::QueryRuntimeResourceID(const std::string& idResource)
{
    ZRuntimeResourceID runtimeResourceID = ResourceIDRegistry::GetInstance().GetRuntimeResourceID(idResource);

    return runtimeResourceID;
}

const std::string& ZRuntimeResourceID::QueryResourceID(const ZRuntimeResourceID& ridResource)
{
    return ResourceIDRegistry::GetInstance().GetResourceID(ridResource);
}

bool ZRuntimeResourceID::operator==(const ZRuntimeResourceID& other) const
{
	return GetID() == other.GetID();
}

bool ZRuntimeResourceID::operator!=(const ZRuntimeResourceID& other) const
{
	return GetID() != other.GetID();
}

void ZRuntimeResourceID::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    /*writer.String("m_IDHigh");
    writer.String(ZUtility::ConvertValueToHexString(m_IDHigh).c_str());
    writer.String("m_IDLow");
    writer.String(ZUtility::ConvertValueToHexString(m_IDLow).c_str());*/
    writer.String("resourceID");
    writer.String(QueryResourceID(*this).c_str());

    writer.EndObject();
}

void ZRuntimeResourceID::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int idHighOffset = offset + offsetof(ZRuntimeResourceID, m_IDHigh);
    unsigned int idLowOffset = offset + offsetof(ZRuntimeResourceID, m_IDLow);

    binarySerializer.WriteToMemory(&m_IDHigh, sizeof(unsigned int), idHighOffset);
    binarySerializer.WriteToMemory(&m_IDLow, sizeof(unsigned int), idLowOffset);

    binarySerializer.RecordRuntimeResourceIDOffset(offset);
}

ZRuntimeResourceID* ZRuntimeResourceID::DeserializeFromJson(const rapidjson::Value& object)
{
    //ZRuntimeResourceID* runtimeResourceID = new ZRuntimeResourceID();

    //runtimeResourceID->m_IDHigh = std::strtoul(object["m_IDHigh"].GetString(), nullptr, 16);
    //runtimeResourceID->m_IDLow = std::strtoul(object["m_IDLow"].GetString(), nullptr, 16);

    static std::unordered_map<std::string, unsigned long long> resourceIDs;

    if (resourceIDs.empty())
    {
        std::string line;
        char buffer[MAX_PATH];

        GetModuleFileNameA(NULL, buffer, MAX_PATH);

        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        std::ifstream ifstream = std::ifstream(std::format("{}/HashMap_Release.txt", std::string(buffer).substr(0, pos)));

        if (ifstream.is_open())
        {
            ifstream.seekg(0, ifstream.end);

            size_t fileSize = static_cast<size_t>(ifstream.tellg());

            ifstream.seekg(0, ifstream.beg);

            std::vector<char> hashListData = std::vector<char>(fileSize, 0);
            unsigned int position = 0, lastPosition = 0;

            ifstream.read(hashListData.data(), fileSize);

            while (true)
            {
                if (hashListData.data()[position] == 0x0A)
                {
                    hashListData.data()[position] = 0;

                    std::string line = std::string(&hashListData.data()[lastPosition]);

                    unsigned long long hash = std::stoull(line.substr(0, line.find(' ')), nullptr, 16);
                    std::string resourceID = line.substr(line.find(' ') + 1);

                    resourceIDs.insert(std::make_pair(resourceID, hash));

                    lastPosition = position + 1;
                }

                position++;

                if (position > fileSize)
                {
                    break;
                }
            }

            ifstream.close();
        }
    }

    std::string resourceID = object["resourceID"].GetString();

    if (!resourceID.empty())
    {
        return new ZRuntimeResourceID(resourceIDs[resourceID]);
    }

    return new ZRuntimeResourceID(-1);
}
