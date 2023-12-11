#pragma once

namespace CloakWorks
{
    namespace Reflection
    {
        enum class FieldType
        {
            kFieldType_Invalid = -1,
            kFieldType_bool = 0,
            kFieldType_byte = 1,
            kFieldType_char = 2,
            kFieldType_uint16 = 3,
            kFieldType_int16 = 4,
            kFieldType_uint32 = 5,
            kFieldType_int32 = 6,
            kFieldType_float = 7,
            kFieldType_string = 8,
            kFieldType_Object = 9,
            kFieldType_Array = 10,
            kFieldType_Count = 11
        };
    }

    struct BinaryNode
    {
        unsigned int nameOffset;
        unsigned int nameLength;
        unsigned int classNameOffset;
        unsigned int classNameLength;
        unsigned int nextBinaryNodeOffset;
        unsigned int childBinaryNodeOffset;
        unsigned int dataOffset;
        unsigned int dataSize;
        unsigned int childBinaryNodeCount;
        unsigned int arrayPrimitiveCount;
        Reflection::FieldType arrayPrimitiveType;
        unsigned int primitiveCount;
        Reflection::FieldType primitiveType;
    };
}
