#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "../Serializer/ZBinarySerializer.h"
#include "Glacier/ZString.h"

struct STemplateSubEntity;
struct STemplateSubEntityBlueprint;

template <typename T>
class TArray
{
public:
    TArray()
    {
        m_pStart = nullptr;
        m_pEnd = nullptr;
        m_pLast = nullptr;
    }

    TArray(const size_t initialSize)
    {
        m_pStart = static_cast<T*>(operator new(sizeof(T) * initialSize, std::align_val_t(alignof(T))));

        for (size_t i = 0; i < initialSize; ++i)
        {
            new (m_pStart + i) T();
        }

        m_pEnd = m_pStart + initialSize;
        m_pLast = m_pEnd;
    }

    TArray(const TArray& other)
    {
        const size_t size = other.Size();
        m_pStart = static_cast<T*>(operator new(sizeof(T) * size, std::align_val_t(alignof(T))));

        for (size_t i = 0; i < size; ++i)
        {
            new (m_pStart + i) T(other.m_pStart[i]);
        }

        m_pEnd = m_pStart + size;
        m_pLast = m_pEnd;
    }

    TArray(TArray&& other) noexcept : m_pStart(other.m_pStart), m_pEnd(other.m_pEnd), m_pLast(other.m_pLast)
    {
        other.m_pStart = nullptr;
        other.m_pEnd = nullptr;
        other.m_pLast = nullptr;
    }

    ~TArray()
    {
        for (T* p = m_pStart; p != m_pEnd; ++p)
        {
            p->~T();
        }

        operator delete(m_pStart, std::align_val_t(alignof(T)));

        m_pStart = nullptr;
        m_pEnd = nullptr;
        m_pLast = nullptr;
    }

    TArray& operator=(const TArray& other)
    {
        if (this != &other)
        {
            for (T* p = m_pStart; p != m_pEnd; ++p)
            {
                p->~T();
            }

            operator delete(m_pStart, std::align_val_t(alignof(T)));

            size_t size = other.Size();
            m_pStart = static_cast<T*>(operator new(sizeof(T) * size, std::align_val_t(alignof(T))));

            std::copy(other.m_pStart, other.m_pEnd, m_pStart);

            m_pEnd = m_pStart + size;
            m_pLast = m_pEnd;
        }

        return *this;
    }

    TArray& operator=(TArray&& other) noexcept
    {
        if (this != &other)
        {
            for (T* p = m_pStart; p != m_pEnd; ++p)
            {
                p->~T();
            }

            operator delete(m_pStart, std::align_val_t(alignof(T)));

            m_pStart = other.m_pStart;
            m_pEnd = other.m_pEnd;
            m_pLast = other.m_pLast;

            other.m_pStart = nullptr;
            other.m_pEnd = nullptr;
            other.m_pLast = nullptr;
        }

        return *this;
    }

    const T* GetStart() const
    {
        return m_pStart;
    }

    T* GetStart()
    {
        return m_pStart;
    }

    const T* GetEnd() const
    {
        return m_pEnd;
    }

    T* GetEnd()
    {
        return m_pEnd;
    }

    const T* GetLast() const
    {
        return m_pLast;
    }

    T* GetLast()
    {
        return m_pLast;
    }

    void SetStart(T* pStart)
    {
        m_pStart = pStart;
    }

    void SetEnd(T* pEnd)
    {
        m_pEnd = pEnd;
    }

    void Reserve(const size_t capacity)
    {
        if (capacity == 0)
        {
            return;
        }

        if (capacity <= Capacity())
        {
            return;
        }

        T* newStart = static_cast<T*>(operator new(sizeof(T) * capacity, std::align_val_t(alignof(T))));
        size_t currentSize = Size();

        for (size_t i = 0; i < currentSize; ++i)
        {
            new(newStart + i) T(std::move(m_pStart[i]));
        }

        for (size_t i = 0; i < currentSize; ++i)
        {
            m_pStart[i].~T();
        }

        operator delete(m_pStart, std::align_val_t(alignof(T)));

        m_pStart = newStart;
        m_pEnd = m_pStart + currentSize;
        m_pLast = m_pStart + capacity;
    }

    void Resize(const size_t newSize)
    {
        if (newSize == 0)
        {
            return;
        }

        if (newSize > Capacity())
        {
            Reserve(newSize);
        }

        if (newSize < Size())
        {
            for (T* p = m_pStart + newSize; p != m_pEnd; ++p)
            {
                p->~T();
            }
        }
        else if (newSize > Size())
        {
            for (T* p = m_pEnd; p != m_pStart + newSize; ++p)
            {
                new(p) T();
            }
        }

        m_pEnd = m_pStart + newSize;
    }

    const size_t Size() const
    {
        return m_pEnd - m_pStart;
    }

    const size_t Capacity() const
    {
        return m_pLast - m_pStart;
    }

    T& operator[](const size_t index)
    {
        return m_pStart[index];
    }

    const T& operator[](const size_t index) const
    {
        return m_pStart[index];
    }

    void Clear()
    {
        for (T* p = m_pStart; p != m_pEnd; ++p)
        {
            p->~T();
        }

        m_pEnd = m_pStart;
    }

    void PushBack(const T& element)
    {
        if (m_pEnd == m_pLast)
        {
            size_t currentSize = Size();
            size_t newSize = (currentSize == 0) ? 1 : currentSize * 2;

            Reserve(newSize);
        }

        new (m_pEnd) T(element);
        ++m_pEnd;
    }

    void RemoveAt(const size_t index)
    {
        if (index >= Size())
        {
            return;
        }

        T* element = m_pStart + index;

        element->~T();

        if (Size() > 1)
        {
            std::move(element + 1, m_pEnd, element);

            --m_pEnd;
        }
        else
        {
            m_pEnd = m_pStart;
        }
    }

    bool operator==(const TArray& other) const
    {
        if (Size() != other.Size())
        {
            return false;
        }

        const TArray& array = *this;

        for (size_t i = 0; i < Size(); ++i)
        {
            if (array[i] != other[i])
            {
                return false;
            }
        }

        return true;
    }

    void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.StartArray();

        const size_t size = Size();

        for (size_t i = 0; i < size; i++)
        {
            T* element = &this->operator[](i);

            if constexpr (std::is_class_v<T>)
            {
                if constexpr (std::is_same_v<T, STemplateSubEntity> || std::is_same_v<T, STemplateSubEntityBlueprint>)
                {
                    element->SerializeToJson(writer, i);
                }
                else
                {
                    element->SerializeToJson(writer);
                }
            }
            else
            {
                if constexpr (std::is_same_v<T, char> || std::is_same_v<T, short> || std::is_same_v<T, int> || std::is_enum_v<T> || std::is_scoped_enum_v<T>)
                {
                    if constexpr (std::is_scoped_enum_v<T>)
                    {
                        writer.Int(static_cast<int>(*element));
                    }
                    else
                    {
                        writer.Int(*element);
                    }
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

    void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
    {
        const size_t elementsCount = Size();
        unsigned int elementsStartOffset;

        if (elementsCount > 0)
        {
            const unsigned int size = sizeof(T) * elementsCount;

            elementsStartOffset = binarySerializer.ReserveLayoutFor(elementsCount, sizeof(T), alignof(T), 4);

            const unsigned int elementsEndOffset = elementsStartOffset + size;

            binarySerializer.WriteToMemory(&elementsCount, sizeof(unsigned int), elementsStartOffset - 4);

            binarySerializer.WriteToMemory(&elementsStartOffset, sizeof(unsigned int), offset);
            binarySerializer.WriteToMemory(&elementsEndOffset, sizeof(unsigned int), offset + 4);
            binarySerializer.WriteToMemory(&elementsEndOffset, sizeof(unsigned int), offset + 8);
        }

        binarySerializer.RecordOffsetForRebasing(offset);
        binarySerializer.RecordOffsetForRebasing(offset + 4);
        binarySerializer.RecordOffsetForRebasing(offset + 8);

        for (unsigned int i = 0; i < elementsCount; ++i)
        {
            const unsigned int elementOffset = elementsStartOffset + sizeof(T) * i;
            T* type = &this->operator[](i);

            if (i == 0)
            {
                binarySerializer.SetLayoutPointer(binarySerializer.GetAlignedLayoutPointer(binarySerializer.GetLayoutPointer(), alignof(TArray)));
            }

            if constexpr (std::is_class_v<T>)
            {
                type->SerializeToMemory(binarySerializer, elementOffset);
            }
            else
            {
                binarySerializer.WriteToMemory(type, sizeof(T), elementOffset);
            }
        }
    }

    static TArray* DeserializeFromJson(const rapidjson::Value& jsonArray)
    {
        const size_t size = jsonArray.Size();
        TArray* array = new TArray(size);

        for (rapidjson::Value::ConstValueIterator it = jsonArray.Begin(); it != jsonArray.End(); ++it)
        {
            if constexpr (std::is_class_v<T>)
            {
                T* element = nullptr;

                if constexpr (std::is_same_v<T, ZString>)
                {
                    element = T::DeserializeFromJson(it->GetString());
                }
                else
                {
                    if (it->IsObject())
                    {
                        const rapidjson::Value& object = it->GetObj();

                        element = T::DeserializeFromJson(object);
                    }
                    else
                    {
                        const rapidjson::Value& array2 = it->GetArray();

                        element = T::DeserializeFromJson(array2);
                    }
                }

                array->PushBack(*element);
            }
            else
            {
                if constexpr (std::is_same_v<T, char> || std::is_same_v<T, short> || std::is_same_v<T, int>)
                {
                    array->PushBack(it->GetInt());
                }
                else if constexpr (std::is_enum_v<T>)
                {
                    array->PushBack(static_cast<T>(it->GetUint()));
                }
                else if constexpr (std::is_same_v<T, unsigned char> || std::is_same_v<T, unsigned short> || std::is_same_v<T, unsigned int>)
                {
                    array->PushBack(it->GetUint());
                }
                else if constexpr (std::is_same_v<T, long long>)
                {
                    array->PushBack(it->GetInt64());
                }
                else if constexpr (std::is_same_v<T, unsigned long long>)
                {
                    array->PushBack(it->GetUint64());
                }
                else if constexpr (std::is_same_v<T, float>)
                {
                    array->PushBack(it->GetFloat());
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    array->PushBack(it->GetDouble());
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    array->PushBack(it->GetBool());
                }
            }
        }

        return array;
    }

    static void DeserializeFromJson(TArray& array, const rapidjson::Value& jsonArray)
    {
        const size_t size = array.Size();

        array.Resize(size);

        for (rapidjson::Value::ConstValueIterator it = jsonArray.Begin(); it != jsonArray.End(); ++it)
        {
            if constexpr (std::is_class_v<T>)
            {
                T element;

                if constexpr (std::is_same_v<T, ZString>)
                {
                    element = T::DeserializeFromJson(element, it->GetString());
                }
                else
                {
                    if (it->IsObject())
                    {
                        const rapidjson::Value& object = it->GetObj();

                        T::DeserializeFromJson(element, object);
                    }
                    else
                    {
                        const rapidjson::Value& array2 = it->GetArray();

                        T::DeserializeFromJson(element, array2);
                    }
                }

                array.PushBack(element);
            }
            else
            {
                if constexpr (std::is_same_v<T, char> || std::is_same_v<T, short> || std::is_same_v<T, int>)
                {
                    array.PushBack(it->GetInt());
                }
                else if constexpr (std::is_enum_v<T>)
                {
                    array.PushBack(static_cast<T>(it->GetUint()));
                }
                else if constexpr (std::is_same_v<T, unsigned char> || std::is_same_v<T, unsigned short> || std::is_same_v<T, unsigned int>)
                {
                    array.PushBack(it->GetUint());
                }
                else if constexpr (std::is_same_v<T, long long>)
                {
                    array.PushBack(it->GetInt64());
                }
                else if constexpr (std::is_same_v<T, unsigned long long>)
                {
                    array.PushBack(it->GetUint64());
                }
                else if constexpr (std::is_same_v<T, float>)
                {
                    array.PushBack(it->GetFloat());
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    array.PushBack(it->GetDouble());
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    array.PushBack(it->GetBool());
                }
            }
        }
    }

private:
    T* m_pStart;
    T* m_pEnd;
    T* m_pLast;
};
