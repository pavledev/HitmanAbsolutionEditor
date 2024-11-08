#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

class ZString;

template <typename TKey, typename TValue>
class TPair
{
public:
	TPair() = default;

	TPair(const TKey& key, const TValue& value) : m_key(key), m_value(value)
	{
	}

	const TKey& Key() const
	{
		return m_key;
	}

	const TValue& Value() const
	{
		return m_value;
	}

	TKey& Key()
	{
		return m_key;
	}

	TValue& Value()
	{
		return m_value;
	}

	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
	{
		writer.StartObject();

		writer.String("m_key");

		if constexpr (std::is_class_v<TKey>)
		{
			m_key.SerializeToJson(writer);
		}
		else
		{
			if constexpr (std::is_same_v<TKey, char> || std::is_same_v<TKey, short> || std::is_same_v<TKey, int> || std::is_enum_v<TKey>)
			{
				writer.Int(m_key);
			}
			else if constexpr (std::is_same_v<TKey, unsigned char> || std::is_same_v<TKey, unsigned short> || std::is_same_v<TKey, unsigned int>)
			{
				writer.Uint(m_key);
			}
			else if constexpr (std::is_same_v<TKey, long long>)
			{
				writer.Int64(m_key);
			}
			else if constexpr (std::is_same_v<TKey, unsigned long long>)
			{
				writer.Uint64(m_key);
			}
			else if constexpr (std::is_same_v<TKey, float> || std::is_same_v<TKey, double>)
			{
				writer.Double(m_key);
			}
			else if constexpr (std::is_same_v<TKey, bool>)
			{
				writer.Bool(m_key);
			}
		}

		writer.String("m_value");

		if constexpr (std::is_class_v<TValue>)
		{
			m_value.SerializeToJson(writer);
		}
		else
		{
			if constexpr (std::is_same_v<TValue, char> || std::is_same_v<TValue, short> || std::is_same_v<TValue, int> || std::is_enum_v<TValue>)
			{
				writer.Int(m_value);
			}
			else if constexpr (std::is_same_v<TValue, unsigned char> || std::is_same_v<TValue, unsigned short> || std::is_same_v<TValue, unsigned int>)
			{
				writer.Uint(m_value);
			}
			else if constexpr (std::is_same_v<TValue, long long>)
			{
				writer.Int64(m_value);
			}
			else if constexpr (std::is_same_v<TValue, unsigned long long>)
			{
				writer.Uint64(m_value);
			}
			else if constexpr (std::is_same_v<TValue, float> || std::is_same_v<TValue, double>)
			{
				writer.Double(m_value);
			}
			else if constexpr (std::is_same_v<TValue, bool>)
			{
				writer.Bool(m_value);
			}
		}

		writer.EndObject();
	}

	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
	{
		unsigned int keyOffset = offset + offsetof(TPair, m_key);
		unsigned int valueOffset = offset + offsetof(TPair, m_value);

		if constexpr (std::is_class_v<TKey>)
		{
			m_key.SerializeToMemory(binarySerializer, keyOffset);
		}
		else
		{
			binarySerializer.WriteToMemory(&m_key, sizeof(TKey), keyOffset);
		}

		if constexpr (std::is_class_v<TValue>)
		{
			m_value.SerializeToMemory(binarySerializer, valueOffset);
		}
		else
		{
			binarySerializer.WriteToMemory(&m_value, sizeof(TValue), valueOffset);
		}
	}

	static TPair* DeserializeFromJson(const rapidjson::Value& object)
	{
		TPair* pair = new TPair();

		if constexpr (std::is_class_v<TKey>)
		{
			if constexpr (std::is_same_v<TKey, ZString>)
			{
				pair->m_key = *TKey::DeserializeFromJson(object["m_key"].GetString());
			}
			else
			{
				pair->m_key = *TKey::DeserializeFromJson(object);
			}
		}
		else
		{
			if constexpr (std::is_same_v<TKey, char> || std::is_same_v<TKey, short> || std::is_same_v<TKey, int> || std::is_enum_v<TKey>)
			{
				pair->m_key = object["m_key"].GetInt();
			}
			else if constexpr (std::is_same_v<TKey, unsigned char> || std::is_same_v<TKey, unsigned short> || std::is_same_v<TKey, unsigned int>)
			{
				pair->m_key = object["m_key"].GetUint();
			}
			else if constexpr (std::is_same_v<TKey, long long>)
			{
				pair->m_key = object["m_key"].GetInt64();
			}
			else if constexpr (std::is_same_v<TKey, unsigned long long>)
			{
				pair->m_key = object["m_key"].GetUint64();
			}
			else if constexpr (std::is_same_v<TKey, float>)
			{
				pair->m_key = object["m_key"].GetFloat();
			}
			else if constexpr (std::is_same_v<TKey, double>)
			{
				pair->m_key = object["m_key"].GetDouble();
			}
			else if constexpr (std::is_same_v<TKey, bool>)
			{
				pair->m_key = object["m_key"].GetBool();
			}
		}

		if constexpr (std::is_class_v<TValue>)
		{
			pair->m_value = *TValue::DeserializeFromJson(object);
		}
		else
		{
			if constexpr (std::is_same_v<TValue, char> || std::is_same_v<TValue, short> || std::is_same_v<TValue, int> || std::is_enum_v<TValue>)
			{
				pair->m_value = object["m_value"].GetInt();
			}
			else if constexpr (std::is_same_v<TValue, unsigned char> || std::is_same_v<TValue, unsigned short> || std::is_same_v<TValue, unsigned int>)
			{
				pair->m_value = object["m_value"].GetUint();
			}
			else if constexpr (std::is_same_v<TValue, long long>)
			{
				pair->m_value = object["m_value"].GetInt64();
			}
			else if constexpr (std::is_same_v<TValue, unsigned long long>)
			{
				pair->m_value = object["m_value"].GetUint64();
			}
			else if constexpr (std::is_same_v<TValue, float>)
			{
				pair->m_value = object["m_value"].GetFloat();
			}
			else if constexpr (std::is_same_v<TValue, double>)
			{
				pair->m_value = object["m_value"].GetDouble();
			}
			else if constexpr (std::is_same_v<TValue, bool>)
			{
				pair->m_value = object["m_value"].GetBool();
			}
		}

		return pair;
	}

private:
	TKey m_key;
	TValue m_value;
};
