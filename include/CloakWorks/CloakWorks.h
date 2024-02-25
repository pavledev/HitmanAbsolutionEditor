#pragma once

#include <vector>

#include "Math/Vector3.h"
#include "Math/Vector2.h"

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

    class ShapeDefinition
    {
    public:
        unsigned int GetNumRows() const;
        unsigned int GetNumColumns() const;
        unsigned int GetNumNodes() const;
        unsigned int GetNumEvenRows() const;
        unsigned int GetNumOddRows() const;
        unsigned int GetRowStartIndex(const unsigned int rowIndex) const;
        const std::vector<float>& GetStartingPositions() const;
        std::vector<float>& GetStartingNormals();
        const std::vector<int>& GetStartingFlags() const;
        void SetNumRows(const unsigned int numRows);
        void SetNumColumns(const unsigned int numColumns);
        void SetNumNodes(const unsigned int numNodes);
        void SetStartingPositions(std::vector<float>& startingPositions);
        void SetStartingFlags(std::vector<int>& startingFlags);

    private:
        unsigned int numRows;
        unsigned int numColumns;
        unsigned int numNodes;
        std::vector<float> startingPositions;
        std::vector<float> startingNormals;
        std::vector<int> startingFlags;
    };

    class ClothNormalsUpdater
    {
    public:
        static void CalcNormalsForStream(const std::vector<float>& startingPositions, std::vector<float>& startingNormals, const std::vector<int>& startingFlags, unsigned int index1, unsigned int index2, unsigned int count, const unsigned int columnCount);
    };

    class SheetShapeDefinition
    {
    public:
        static std::vector<Vector3> GenerateStartingNormals(CloakWorks::ShapeDefinition& shapeDefinition);
    };

    class SheetMeshControlInstance
    {
    public:
        SheetMeshControlInstance(ShapeDefinition& shapeDefinition);
        const unsigned int GetNumVerts() const;
        const unsigned int GetNumIndices() const;

        template <typename T>
        void FillIndexBuffer(std::vector<T>& indices) const
        {
            const std::vector<int>& startingFlags = shapeDefinition.GetStartingFlags();
            const unsigned int rowCount = shapeDefinition.GetNumRows();
            const unsigned int columnCount = shapeDefinition.GetNumColumns();
            unsigned int index = 0;

            indices.resize(GetNumIndices());

            for (unsigned int row = 0; row < rowCount - 1; ++row)
            {
                for (unsigned int column = 0; column < columnCount - 1; ++column)
                {
                    const unsigned int index1 = column + shapeDefinition.GetRowStartIndex(row);
                    const unsigned int index2 = column + shapeDefinition.GetRowStartIndex(row + 1);

                    if ((startingFlags[index1] & 8) == 0
                        && (startingFlags[index1 + 1] & 8) == 0
                        && (startingFlags[index2] & 8) == 0
                        && (startingFlags[index2 + 1] & 8) == 0)
                    {
                        indices[index] = index1;
                        indices[index + 1] = index2;
                        indices[index + 2] = index2 + 1;
                        indices[index + 3] = index2 + 1;
                        indices[index + 4] = index1 + 1;
                        indices[index + 5] = index1;

                        index += 6;
                    }
                }
            }
        }

        void FillTexCoordsBuffer(std::vector<Vector2>& textureCoordinates) const;

    private:
        ShapeDefinition& shapeDefinition;
    };

    class TubeShapeDefinition
    {
    public:
        static std::vector<Vector3> GenerateStartingNormals(CloakWorks::ShapeDefinition& shapeDefinition);
    };

    class StrandShapeDefinition
    {
    public:
        static std::vector<Vector3> GenerateStartingNormals(CloakWorks::ShapeDefinition& shapeDefinition);
    };

    class StrandMeshControlInstance
    {
    public:
        StrandMeshControlInstance(ShapeDefinition& shapeDefinition);
        const unsigned int GetNumVerts() const;
        const unsigned int GetNumIndices() const;

        template <typename T>
        void FillIndexBuffer(std::vector<T>& indices) const
        {
            const std::vector<int>& startingFlags = shapeDefinition.GetStartingFlags();
            const unsigned int rowCount = shapeDefinition.GetNumRows();
            const unsigned int columnCount = shapeDefinition.GetNumColumns();
            unsigned int index = 0;

            indices.resize(GetNumIndices());

            for (unsigned int row = 0; row < rowCount - 1; ++row)
            {
                const unsigned int currentRowStartIndex = shapeDefinition.GetRowStartIndex(row);
                const unsigned int nextRowStartIndexNext = shapeDefinition.GetRowStartIndex(row + 1);

                for (unsigned int column = 0; column < columnCount; ++column)
                {
                    const int currentFlag = startingFlags[currentRowStartIndex + column];
                    const int nextFlag = startingFlags[nextRowStartIndexNext + column];

                    if ((currentFlag & 8) == 0 && (nextFlag & 8) == 0)
                    {
                        const unsigned int currentIndex = 2 * (currentRowStartIndex + column);
                        const unsigned int nextIndex = 2 * (nextRowStartIndexNext + column);

                        indices[index] = currentIndex;
                        indices[index + 1] = currentIndex + 1;
                        indices[index + 2] = nextIndex;
                        indices[index + 3] = nextIndex + 1;
                        indices[index + 4] = nextIndex;
                        indices[index + 5] = currentIndex + 1;

                        index += 6;
                    }
                }
            }
        }

        void FillTexCoordsBuffer(std::vector<Vector2>& textureCoordinates) const;

    private:
        ShapeDefinition& shapeDefinition;
    };

    class TubeMeshControlInstance
    {
    public:
        TubeMeshControlInstance(ShapeDefinition& shapeDefinition);
        const unsigned int GetNumVerts() const;
        const unsigned int GetNumIndices() const;

        template <typename T>
        void FillIndexBuffer(std::vector<T>& indices) const
        {
            const std::vector<int>& startingFlags = shapeDefinition.GetStartingFlags();
            const unsigned int rowCount = shapeDefinition.GetNumRows();
            const unsigned int columnCount = shapeDefinition.GetNumColumns();
            unsigned int index = 0;

            indices.resize(GetNumIndices());

            for (unsigned int row = 0; row < rowCount; ++row)
            {
                if (columnCount <= 1)
                {
                    continue;
                }

                const unsigned int currentRowStartIndex = shapeDefinition.GetRowStartIndex(row);
                const unsigned int nextRowStartIndex = shapeDefinition.GetRowStartIndex((row + 1) % rowCount);

                for (unsigned int column = 0; column < columnCount - 1; ++column)
                {
                    const unsigned int currentIndex = currentRowStartIndex + column;
                    const unsigned int nextIndex = nextRowStartIndex + column;

                    if ((startingFlags[currentIndex] & 8) == 0
                        && (startingFlags[currentIndex + 1] & 8) == 0
                        && (startingFlags[nextIndex] & 8) == 0
                        && (startingFlags[nextIndex + 1] & 8) == 0)
                    {
                        indices[index] = currentIndex;
                        indices[index + 1] = nextIndex;
                        indices[index + 2] = nextIndex + 1;
                        indices[index + 3] = nextIndex + 1;
                        indices[index + 4] = currentIndex + 1;
                        indices[index + 5] = currentIndex;

                        index += 6;
                    }
                }
            }
        }

        void FillTexCoordsBuffer(std::vector<Vector2>& textureCoordinates) const;

    private:
        ShapeDefinition& shapeDefinition;
    };
}
