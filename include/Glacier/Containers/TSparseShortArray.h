#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

template <typename T>
class TSparseShortArray
{
public:
    bool Contains(int index) const
    {
        return indices[index] != -1;
    }

    T& operator[](int index) const
    {
        return items[indices[index]];
    }

    void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.StartArray();

        for (int i = 0; i < size; i++)
        {
            if (!Contains(i))
            {
                continue;
            }

            T* element = &this->operator[](i);

            if constexpr (std::is_class_v<T>)
            {
                element->SerializeToJson(writer);
            }
            else
            {
                if constexpr (std::is_same_v<T, char> || std::is_same_v<T, short> || std::is_same_v<T, int> || std::is_enum_v<T>)
                {
                    writer.Int(*element);
                }
                else if constexpr (std::is_same_v<T, unsigned char> || std::is_same_v<T, unsigned short> || std::is_same_v<T, unsigned int>)
                {
                    writer.Uint(*element);
                }
                else if constexpr (std::is_same_v<T, long long>)
                {
                    writer.Int64(*element);
                }
                else if constexpr (std::is_same_v<T, unsigned long long>)
                {
                    writer.Uint64(*element);
                }
                else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
                {
                    writer.Double(*element);
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    writer.Bool(*element);
                }
            }
        }

        writer.EndArray();
    }

private:
    int size;
    char* indices;
    T* items;
};
