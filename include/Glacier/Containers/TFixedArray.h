#pragma once

#include "TFixedArrayBase.h"
#include "ZFixedArrayData.h"

template <typename TElement, unsigned int TSize>
class TFixedArray : public TFixedArrayBase<TElement, ZFixedArrayData<TElement, TSize>>
{
public:
    unsigned int Size() const
    {
        return TSize;
    }

    TElement& operator[](unsigned int nIndex)
    {
        return this->m_pStart[nIndex];
    }

    const TElement& operator[](unsigned int nIndex) const
    {
        return this->m_pStart[nIndex];
    }

    void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.StartArray();

        unsigned int size = Size();

        for (unsigned int i = 0; i < size; i++)
        {
            TElement element = this->operator[](i);

            if constexpr (std::is_class_v<TElement>)
            {
                element.SerializeToJson(writer);
            }
            else
            {
                if constexpr (std::is_same_v<TElement, char> || std::is_same_v<TElement, short> || std::is_same_v<TElement, int>)
                {
                    writer.Int(element);
                }
                else if constexpr (std::is_same_v<TElement, unsigned char> || std::is_same_v<TElement, unsigned short> || std::is_same_v<TElement, unsigned int> || std::is_enum_v<TElement>)
                {
                    writer.Uint(element);
                }
                else if constexpr (std::is_same_v<TElement, long long>)
                {
                    writer.Int64(element);
                }
                else if constexpr (std::is_same_v<TElement, unsigned long long>)
                {
                    writer.Uint64(element);
                }
                else if constexpr (std::is_same_v<TElement, float> || std::is_same_v<TElement, double>)
                {
                    writer.Double(element);
                }
                else if constexpr (std::is_same_v<TElement, bool>)
                {
                    writer.Bool(element);
                }
            }
        }

        writer.EndArray();
    }

    void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
    {
        unsigned int elementsCount = Size();

        for (unsigned int i = 0; i < elementsCount; ++i)
        {
            unsigned int elementOffset = offset + sizeof(TElement) * i;
            TElement element = this->operator[](i);

            if constexpr (std::is_class_v<TElement>)
            {
                element.SerializeToMemory(binarySerializer, elementOffset);
            }
            else
            {
                binarySerializer.WriteToMemory(&element, sizeof(TElement), elementOffset);
            }
        }
    }

    static TFixedArray* DeserializeFromJson(const rapidjson::Value& array2)
    {
        unsigned int size = array2.Size();
        TFixedArray<TElement, TSize>* fixedArray = new TFixedArray<TElement, TSize>();
        int i = 0;

        for (rapidjson::Value::ConstValueIterator it = array2.Begin(); it != array2.End(); ++it)
        {
            if constexpr (std::is_class_v<TElement>)
            {
                const rapidjson::Value& object2 = it->GetObj();
                TElement element = TElement::DeserializeFromJson(object2);

                fixedArray->operator[](i) = element;
            }
            else
            {
                if constexpr (std::is_same_v<TElement, char> || std::is_same_v<TElement, short> || std::is_same_v<TElement, int> || std::is_enum_v<TElement>)
                {
                    fixedArray->operator[](i) = it->GetInt();
                }
                else if constexpr (std::is_same_v<TElement, unsigned char> || std::is_same_v<TElement, unsigned short> || std::is_same_v<TElement, unsigned int>)
                {
                    fixedArray->operator[](i) = it->GetUint();
                }
                else if constexpr (std::is_same_v<TElement, long long>)
                {
                    fixedArray->operator[](i) = it->GetInt64();
                }
                else if constexpr (std::is_same_v<TElement, unsigned long long>)
                {
                    fixedArray->operator[](i) = it->GetUint64();
                }
                else if constexpr (std::is_same_v<TElement, float>)
                {
                    fixedArray->operator[](i) = it->GetFloat();
                }
                else if constexpr (std::is_same_v<TElement, double>)
                {
                    fixedArray->operator[](i) = it->GetDouble();
                }
                else if constexpr (std::is_same_v<TElement, bool>)
                {
                    fixedArray->operator[](i) = it->GetBool();
                }
            }

            i++;
        }

        return fixedArray;
    }

    bool operator==(const TFixedArray& other) const
    {
        for (unsigned int i = 0; i < Size(); ++i)
        {
            if (this->operator[](i) != other[i])
            {
                return false;
            }
        }

        return true;
    }
};
