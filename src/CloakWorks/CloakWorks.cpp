#include "CloakWorks/CloakWorks.h"

unsigned int CloakWorks::ShapeDefinition::GetNumRows() const
{
    return numRows;
}

unsigned int CloakWorks::ShapeDefinition::GetNumColumns() const
{
    return numColumns;
}

unsigned int CloakWorks::ShapeDefinition::GetNumNodes() const
{
    return numNodes;
}

unsigned int CloakWorks::ShapeDefinition::GetNumEvenRows() const
{
    return numRows - GetNumOddRows();
}

unsigned int CloakWorks::ShapeDefinition::GetNumOddRows() const
{
    return numRows / 2;
}

unsigned int CloakWorks::ShapeDefinition::GetRowStartIndex(const unsigned int rowIndex) const
{
    const unsigned int numEvenRows = GetNumEvenRows();

    return ((rowIndex >> 1) + numEvenRows * (rowIndex & 1)) * numColumns;
}

const std::vector<float>& CloakWorks::ShapeDefinition::GetStartingPositions() const
{
    return startingPositions;
}

std::vector<float>& CloakWorks::ShapeDefinition::GetStartingNormals()
{
    return startingNormals;
}

const std::vector<int>& CloakWorks::ShapeDefinition::GetStartingFlags() const
{
    return startingFlags;
}

void CloakWorks::ShapeDefinition::SetNumRows(const unsigned int numRows)
{
    this->numRows = numRows;
}

void CloakWorks::ShapeDefinition::SetNumColumns(const unsigned int numColumns)
{
    this->numColumns = numColumns;
}

void CloakWorks::ShapeDefinition::SetNumNodes(const unsigned int numNodes)
{
    this->numNodes = numNodes;
}

void CloakWorks::ShapeDefinition::SetStartingPositions(std::vector<float>& startingPositions)
{
    this->startingPositions = startingPositions;
}

void CloakWorks::ShapeDefinition::SetStartingFlags(std::vector<int>& startingFlags)
{
    this->startingFlags = startingFlags;
}

void CloakWorks::ClothNormalsUpdater::CalcNormalsForStream(const std::vector<float>& startingPositions, std::vector<float>& startingNormals, const std::vector<int>& startingFlags, unsigned int index1, unsigned int index2, unsigned int count, const unsigned int columnCount)
{
    Vector3 previousStartingNormal1;
    Vector3 previousStartingNormal2;
    Vector3 previousStartingNormal3;
    Vector3 previousStartingNormal4;
    Vector3 previousStartingNormal5;
    Vector3 previousStartingNormal6;
    Vector3 previousStartingNormal7;
    Vector3 previousStartingNormal8;

    previousStartingNormal1.x = startingNormals[index1 + 12];
    previousStartingNormal1.y = startingNormals[index1 + 16];
    previousStartingNormal1.z = startingNormals[index1 + 20];

    previousStartingNormal2.x = startingNormals[index1 + 13];
    previousStartingNormal2.y = startingNormals[index1 + 17];
    previousStartingNormal2.z = startingNormals[index1 + 21];

    previousStartingNormal3.x = startingNormals[index1 + 14];
    previousStartingNormal3.y = startingNormals[index1 + 18];
    previousStartingNormal3.z = startingNormals[index1 + 22];

    previousStartingNormal4.x = startingNormals[index1 + 15];
    previousStartingNormal4.y = startingNormals[index1 + 19];
    previousStartingNormal4.z = startingNormals[index1 + 23];

    previousStartingNormal5.x = startingNormals[index2 + 12];
    previousStartingNormal5.y = startingNormals[index2 + 16];
    previousStartingNormal5.z = startingNormals[index2 + 20];

    previousStartingNormal6.x = startingNormals[index2 + 13];
    previousStartingNormal6.y = startingNormals[index2 + 17];
    previousStartingNormal6.z = startingNormals[index2 + 21];

    previousStartingNormal7.x = startingNormals[index2 + 14];
    previousStartingNormal7.y = startingNormals[index2 + 18];
    previousStartingNormal7.z = startingNormals[index2 + 22];

    previousStartingNormal8.x = startingNormals[index2 + 15];
    previousStartingNormal8.y = startingNormals[index2 + 19];
    previousStartingNormal8.z = startingNormals[index2 + 23];

    Vector3 previousNormal1;
    Vector3 previousNormal2;
    Vector3 previousNormal3;
    Vector3 previousNormal4;
    Vector3 previousNormal5;
    Vector3 previousNormal6;
    Vector3 previousNormal7;
    Vector3 previousNormal8;

    previousNormal1.x = startingNormals[index1];
    previousNormal1.y = startingNormals[index1 + 4];
    previousNormal1.z = startingNormals[index1 + 8];

    previousNormal2.x = startingNormals[index1 + 1];
    previousNormal2.y = startingNormals[index1 + 5];
    previousNormal2.z = startingNormals[index1 + 9];

    previousNormal3.x = startingNormals[index1 + 2];
    previousNormal3.y = startingNormals[index1 + 6];
    previousNormal3.z = startingNormals[index1 + 10];

    previousNormal4.x = startingNormals[index1 + 3];
    previousNormal4.y = startingNormals[index1 + 7];
    previousNormal4.z = startingNormals[index1 + 11];

    previousNormal5.x = startingNormals[index2];
    previousNormal5.y = startingNormals[index2 + 4];
    previousNormal5.z = startingNormals[index2 + 8];

    previousNormal6.x = startingNormals[index2 + 1];
    previousNormal6.y = startingNormals[index2 + 5];
    previousNormal6.z = startingNormals[index2 + 9];

    previousNormal7.x = startingNormals[index2 + 2];
    previousNormal7.y = startingNormals[index2 + 6];
    previousNormal7.z = startingNormals[index2 + 10];

    previousNormal8.x = startingNormals[index2 + 3];
    previousNormal8.y = startingNormals[index2 + 7];
    previousNormal8.z = startingNormals[index2 + 11];

    unsigned int startingFlagIndex1 = index1 / 3;
    unsigned int startingFlagIndex2 = index2 / 3;

    int startingFlag1 = startingFlags[startingFlagIndex1];
    int startingFlag2 = startingFlags[startingFlagIndex1 + 1];
    int startingFlag3 = startingFlags[startingFlagIndex1 + 2];
    int startingFlag4 = startingFlags[startingFlagIndex1 + 3];
    int startingFlag5 = startingFlags[startingFlagIndex1 + 4];
    int startingFlag6 = startingFlags[startingFlagIndex1 + 5];
    int startingFlag7 = startingFlags[startingFlagIndex1 + 6];
    int startingFlag8 = startingFlags[startingFlagIndex1 + 7];

    int startingFlag9 = startingFlags[startingFlagIndex2];
    int startingFlag10 = startingFlags[startingFlagIndex2 + 1];
    int startingFlag11 = startingFlags[startingFlagIndex2 + 2];
    int startingFlag12 = startingFlags[startingFlagIndex2 + 3];
    int startingFlag13 = startingFlags[startingFlagIndex2 + 4];
    int startingFlag14 = startingFlags[startingFlagIndex2 + 5];
    int startingFlag15 = startingFlags[startingFlagIndex2 + 6];
    int startingFlag16 = startingFlags[startingFlagIndex2 + 7];

    const unsigned int count2 = count;
    unsigned int n = 0;
    const unsigned int m = columnCount / 4 - 1;

    if (count != 1)
    {
        while (count-- != 1)
        {
            Vector3 vector1, vector2, vector3, vector4,
                vector5, vector6, vector7, vector8,
                vector9, vector10, vector11, vector12,
                vector13, vector14, vector15, vector16;

            vector1.x = startingPositions[index1];
            vector1.y = startingPositions[index1 + 4];
            vector1.z = startingPositions[index1 + 8];

            vector2.x = startingPositions[index1 + 1];
            vector2.y = startingPositions[index1 + 5];
            vector2.z = startingPositions[index1 + 9];

            vector3.x = startingPositions[index1 + 2];
            vector3.y = startingPositions[index1 + 6];
            vector3.z = startingPositions[index1 + 10];

            vector4.x = startingPositions[index1 + 3];
            vector4.y = startingPositions[index1 + 7];
            vector4.z = startingPositions[index1 + 11];

            vector5.x = startingPositions[index1 + 1];
            vector5.y = startingPositions[index1 + 5];
            vector5.z = startingPositions[index1 + 9];

            vector6.x = startingPositions[index1 + 2];
            vector6.y = startingPositions[index1 + 6];
            vector6.z = startingPositions[index1 + 10];

            vector7.x = startingPositions[index1 + 3];
            vector7.y = startingPositions[index1 + 7];
            vector7.z = startingPositions[index1 + 11];

            vector8.x = startingPositions[index1 + 12];
            vector8.y = startingPositions[index1 + 16];
            vector8.z = startingPositions[index1 + 20];

            vector9.x = startingPositions[index2];
            vector9.y = startingPositions[index2 + 4];
            vector9.z = startingPositions[index2 + 8];

            vector10.x = startingPositions[index2 + 1];
            vector10.y = startingPositions[index2 + 5];
            vector10.z = startingPositions[index2 + 9];

            vector11.x = startingPositions[index2 + 2];
            vector11.y = startingPositions[index2 + 6];
            vector11.z = startingPositions[index2 + 10];

            vector12.x = startingPositions[index2 + 3];
            vector12.y = startingPositions[index2 + 7];
            vector12.z = startingPositions[index2 + 11];

            vector13.x = startingPositions[index2 + 1];
            vector13.y = startingPositions[index2 + 5];
            vector13.z = startingPositions[index2 + 9];

            vector14.x = startingPositions[index2 + 2];
            vector14.y = startingPositions[index2 + 6];
            vector14.z = startingPositions[index2 + 10];

            vector15.x = startingPositions[index2 + 3];
            vector15.y = startingPositions[index2 + 7];
            vector15.z = startingPositions[index2 + 11];

            vector16.x = startingPositions[index2 + 12];
            vector16.y = startingPositions[index2 + 16];
            vector16.z = startingPositions[index2 + 20];

            startingFlag1 = startingFlags[startingFlagIndex1];
            startingFlag2 = startingFlags[startingFlagIndex1 + 1];
            startingFlag3 = startingFlags[startingFlagIndex1 + 2];
            startingFlag4 = startingFlags[startingFlagIndex1 + 3];
            startingFlag5 = startingFlags[startingFlagIndex1 + 4];
            startingFlag6 = startingFlags[startingFlagIndex1 + 5];
            startingFlag7 = startingFlags[startingFlagIndex1 + 6];
            startingFlag8 = startingFlags[startingFlagIndex1 + 7];

            startingFlag9 = startingFlags[startingFlagIndex2];
            startingFlag10 = startingFlags[startingFlagIndex2 + 1];
            startingFlag11 = startingFlags[startingFlagIndex2 + 2];
            startingFlag12 = startingFlags[startingFlagIndex2 + 3];
            startingFlag13 = startingFlags[startingFlagIndex2 + 4];
            startingFlag14 = startingFlags[startingFlagIndex2 + 5];
            startingFlag15 = startingFlags[startingFlagIndex2 + 6];
            startingFlag16 = startingFlags[startingFlagIndex2 + 7];

            previousStartingNormal1.x = startingNormals[index1 + 12];
            previousStartingNormal1.y = startingNormals[index1 + 16];
            previousStartingNormal1.z = startingNormals[index1 + 20];

            previousStartingNormal2.x = startingNormals[index1 + 13];
            previousStartingNormal2.y = startingNormals[index1 + 17];
            previousStartingNormal2.z = startingNormals[index1 + 21];

            previousStartingNormal3.x = startingNormals[index1 + 14];
            previousStartingNormal3.y = startingNormals[index1 + 18];
            previousStartingNormal3.z = startingNormals[index1 + 22];

            previousStartingNormal4.x = startingNormals[index1 + 15];
            previousStartingNormal4.y = startingNormals[index1 + 19];
            previousStartingNormal4.z = startingNormals[index1 + 23];

            previousStartingNormal5.x = startingNormals[index2 + 12];
            previousStartingNormal5.y = startingNormals[index2 + 16];
            previousStartingNormal5.z = startingNormals[index2 + 20];

            previousStartingNormal6.x = startingNormals[index2 + 13];
            previousStartingNormal6.y = startingNormals[index2 + 17];
            previousStartingNormal6.z = startingNormals[index2 + 21];

            previousStartingNormal7.x = startingNormals[index2 + 14];
            previousStartingNormal7.y = startingNormals[index2 + 18];
            previousStartingNormal7.z = startingNormals[index2 + 22];

            previousStartingNormal8.x = startingNormals[index2 + 15];
            previousStartingNormal8.y = startingNormals[index2 + 19];
            previousStartingNormal8.z = startingNormals[index2 + 23];

            Vector3 edge1 = vector2 - vector1;
            Vector3 edge2 = vector3 - vector2;
            Vector3 edge3 = vector4 - vector3;
            Vector3 edge4 = vector8 - vector4;

            Vector3 edge5 = vector9 - vector1;
            Vector3 edge6 = vector10 - vector2;
            Vector3 edge7 = vector11 - vector3;
            Vector3 edge8 = vector12 - vector4;

            Vector3 edge9 = vector13 - vector1;
            Vector3 edge10 = vector14 - vector2;
            Vector3 edge11 = vector15 - vector3;
            Vector3 edge12 = vector16 - vector4;

            Vector3 normal1 = Vector3::Cross(edge5, edge9);
            Vector3 normal2 = Vector3::Cross(edge6, edge9);
            Vector3 normal3 = Vector3::Cross(edge6, edge10);
            Vector3 normal4 = Vector3::Cross(edge7, edge10);
            Vector3 normal5 = Vector3::Cross(edge7, edge11);
            Vector3 normal6 = Vector3::Cross(edge8, edge11);
            Vector3 normal7 = Vector3::Cross(edge8, edge12);
            Vector3 normal8 = Vector3::Cross(edge12, edge4);

            normal1.Normalize();
            normal2.Normalize();
            normal3.Normalize();
            normal4.Normalize();
            normal5.Normalize();
            normal6.Normalize();
            normal7.Normalize();
            normal8.Normalize();

            if (n == m)
            {
                normal7 = Vector3(0, 0, 0);
                normal8 = Vector3(0, 0, 0);

                n = 0;
            }
            else
            {
                ++n;
            }

            if ((startingFlag1 & 8 | startingFlag9 & 8 | startingFlag2 & 8) != 0)
            {
                normal2 = Vector3(0, 0, 0);
            }

            if ((startingFlag2 & 8 | startingFlag10 & 8 | startingFlag3 & 8) != 0)
            {
                normal4 = Vector3(0, 0, 0);
            }

            if ((startingFlag3 & 8 | startingFlag11 & 8 | startingFlag4 & 8) != 0)
            {
                normal6 = Vector3(0, 0, 0);
            }

            if ((startingFlag4 & 8 | startingFlag12 & 8 | startingFlag5 & 8) != 0)
            {
                normal8 = Vector3(0, 0, 0);
            }

            if ((startingFlag1 & 8 | startingFlag9 & 8 | startingFlag10 & 8) != 0)
            {
                normal1 = Vector3(0, 0, 0);
            }

            if ((startingFlag2 & 8 | startingFlag10 & 8 | startingFlag11 & 8) != 0)
            {
                normal3 = Vector3(0, 0, 0);
            }

            if ((startingFlag3 & 8 | startingFlag11 & 8 | startingFlag12 & 8) != 0)
            {
                normal5 = Vector3(0, 0, 0);
            }

            if ((startingFlag4 & 8 | startingFlag12 & 8 | startingFlag13 & 8) != 0)
            {
                normal7 = Vector3(0, 0, 0);
            }

            Vector3 startingNormal1 = normal1 + normal2 + previousNormal1;
            Vector3 startingNormal2 = normal2 + normal3 + normal4 + previousNormal2;
            Vector3 startingNormal3 = normal4 + normal5 + normal6 + previousNormal3;
            Vector3 startingNormal4 = normal6 + normal7 + normal8 + previousNormal4;
            Vector3 startingNormal5 = normal1 + previousNormal5;
            Vector3 startingNormal6 = normal1 + normal3 + previousNormal6;
            Vector3 startingNormal7 = normal3 + normal5 + previousNormal7;
            Vector3 startingNormal8 = normal5 + normal7 + previousNormal8;

            Vector3 startingNormals3[] = { startingNormal1, startingNormal2, startingNormal3, startingNormal4 };
            Vector3 startingNormals4[] = { startingNormal5, startingNormal6, startingNormal7, startingNormal8 };

            for (unsigned int i = 0; i < 4; ++i)
            {
                startingNormals[index1 + i] = startingNormals3[i].x;
                startingNormals[index1 + i + 4] = startingNormals3[i].y;
                startingNormals[index1 + i + 8] = startingNormals3[i].z;
            }

            for (unsigned int i = 0; i < 4; ++i)
            {
                startingNormals[index2 + i] = startingNormals4[i].x;
                startingNormals[index2 + i + 4] = startingNormals4[i].y;
                startingNormals[index2 + i + 8] = startingNormals4[i].z;
            }

            previousNormal1 = normal8 + previousStartingNormal1;
            previousNormal2 = previousStartingNormal2;
            previousNormal3 = previousStartingNormal3;
            previousNormal4 = previousStartingNormal4;
            previousNormal5 = normal7 + previousStartingNormal5;
            previousNormal6 = previousStartingNormal6;
            previousNormal7 = previousStartingNormal7;
            previousNormal8 = previousStartingNormal8;

            index1 += 12;
            index2 += 12;

            startingFlagIndex1 += 4;
            startingFlagIndex2 += 4;
        }
    }

    Vector3 vector1, vector2, vector3, vector4,
        vector5, vector6, vector7, vector8,
        vector9, vector10, vector11, vector12,
        vector13, vector14;

    vector1.x = startingPositions[index1];
    vector1.y = startingPositions[index1 + 4];
    vector1.z = startingPositions[index1 + 8];

    vector2.x = startingPositions[index1 + 1];
    vector2.y = startingPositions[index1 + 5];
    vector2.z = startingPositions[index1 + 9];

    vector3.x = startingPositions[index1 + 2];
    vector3.y = startingPositions[index1 + 6];
    vector3.z = startingPositions[index1 + 10];

    vector4.x = startingPositions[index1 + 3];
    vector4.y = startingPositions[index1 + 7];
    vector4.z = startingPositions[index1 + 11];

    vector5.x = startingPositions[index1 + 1];
    vector5.y = startingPositions[index1 + 5];
    vector5.z = startingPositions[index1 + 9];

    vector6.x = startingPositions[index1 + 2];
    vector6.y = startingPositions[index1 + 6];
    vector6.z = startingPositions[index1 + 10];

    vector7.x = startingPositions[index1 + 3];
    vector7.y = startingPositions[index1 + 7];
    vector7.z = startingPositions[index1 + 11];

    vector8.x = startingPositions[index2];
    vector8.y = startingPositions[index2 + 4];
    vector8.z = startingPositions[index2 + 8];

    vector9.x = startingPositions[index2 + 1];
    vector9.y = startingPositions[index2 + 5];
    vector9.z = startingPositions[index2 + 9];

    vector10.x = startingPositions[index2 + 2];
    vector10.y = startingPositions[index2 + 6];
    vector10.z = startingPositions[index2 + 10];

    vector11.x = startingPositions[index2 + 3];
    vector11.y = startingPositions[index2 + 7];
    vector11.z = startingPositions[index2 + 11];

    vector12.x = startingPositions[index2 + 1];
    vector12.y = startingPositions[index2 + 5];
    vector12.z = startingPositions[index2 + 9];

    vector13.x = startingPositions[index2 + 2];
    vector13.y = startingPositions[index2 + 6];
    vector13.z = startingPositions[index2 + 10];

    vector14.x = startingPositions[index2 + 3];
    vector14.y = startingPositions[index2 + 7];
    vector14.z = startingPositions[index2 + 11];

    Vector3 edge1 = vector2 - vector1;
    Vector3 edge2 = vector3 - vector2;
    Vector3 edge3 = vector4 - vector3;
    Vector3 edge4 = vector1 - vector4;

    Vector3 edge5 = vector8 - vector1;
    Vector3 edge6 = vector9 - vector2;
    Vector3 edge7 = vector10 - vector3;
    Vector3 edge8 = vector11 - vector4;

    Vector3 edge9 = vector9 - vector1;
    Vector3 edge10 = vector10 - vector2;
    Vector3 edge11 = vector11 - vector3;
    Vector3 edge12 = vector8 - vector4;

    Vector3 normal1 = Vector3::Cross(edge5, edge9);
    Vector3 normal2 = Vector3::Cross(edge6, edge9);
    Vector3 normal3 = Vector3::Cross(edge6, edge10);
    Vector3 normal4 = Vector3::Cross(edge7, edge10);
    Vector3 normal5 = Vector3::Cross(edge7, edge11);
    Vector3 normal6 = Vector3::Cross(edge8, edge11);
    Vector3 normal7 = Vector3::Cross(edge8, edge12);
    Vector3 normal8 = Vector3::Cross(edge12, edge4);

    normal1.Normalize();
    normal2.Normalize();
    normal3.Normalize();
    normal4.Normalize();
    normal5.Normalize();
    normal6.Normalize();
    normal7.Normalize();
    normal8.Normalize();

    if (n == m)
    {
        normal7 = Vector3(0, 0, 0);
        normal8 = Vector3(0, 0, 0);
    }

    if ((startingFlag5 & 8 | startingFlag13 & 8 | startingFlag6 & 8) != 0)
    {
        normal2 = Vector3(0, 0, 0);
    }

    if ((startingFlag6 & 8 | startingFlag14 & 8 | startingFlag7 & 8) != 0)
    {
        normal4 = Vector3(0, 0, 0);
    }

    if ((startingFlag7 & 8 | startingFlag15 & 8 | startingFlag8 & 8) != 0)
    {
        normal6 = Vector3(0, 0, 0);
    }

    if ((startingFlag8 & 8 | startingFlag16 & 8 | startingFlag5 & 8) != 0)
    {
        normal8 = Vector3(0, 0, 0);
    }

    if ((startingFlag5 & 8 | startingFlag13 & 8 | startingFlag14 & 8) != 0)
    {
        normal2 = Vector3(0, 0, 0);
    }

    if ((startingFlag6 & 8 | startingFlag14 & 8 | startingFlag15 & 8) != 0)
    {
        normal4 = Vector3(0, 0, 0);
    }

    if ((startingFlag7 & 8 | startingFlag15 & 8 | startingFlag16 & 8) != 0)
    {
        normal6 = Vector3(0, 0, 0);
    }

    if ((startingFlag8 & 8 | startingFlag16 & 8 | startingFlag13 & 8) != 0)
    {
        normal8 = Vector3(0, 0, 0);
    }

    Vector3 startingNormal1 = normal1 + normal2 + previousNormal1;
    Vector3 startingNormal2 = normal2 + normal3 + normal4 + previousNormal2;
    Vector3 startingNormal3 = normal4 + normal5 + normal6 + previousNormal3;
    Vector3 startingNormal4 = normal6 + normal7 + normal8 + previousNormal4;
    Vector3 startingNormal5 = normal1 + previousNormal5;
    Vector3 startingNormal6 = normal1 + normal3 + previousNormal6;
    Vector3 startingNormal7 = normal3 + normal5 + previousNormal7;
    Vector3 startingNormal8 = normal5 + normal7 + previousNormal8;

    Vector3 startingNormals3[] = { startingNormal1, startingNormal2, startingNormal3, startingNormal4 };
    Vector3 startingNormals4[] = { startingNormal5, startingNormal6, startingNormal7, startingNormal8 };

    for (unsigned int i = 0; i < 4; ++i)
    {
        startingNormals[index1 + i] = startingNormals3[i].x;
        startingNormals[index1 + i + 4] = startingNormals3[i].y;
        startingNormals[index1 + i + 8] = startingNormals3[i].z;
    }

    for (unsigned int i = 0; i < 4; ++i)
    {
        startingNormals[index2 + i] = startingNormals4[i].x;
        startingNormals[index2 + i + 4] = startingNormals4[i].y;
        startingNormals[index2 + i + 8] = startingNormals4[i].z;
    }
}

std::vector<Vector3> CloakWorks::SheetShapeDefinition::GenerateStartingNormals(CloakWorks::ShapeDefinition& shapeDefinition)
{
    const std::vector<float>& startingPositions = shapeDefinition.GetStartingPositions();
    std::vector<float>& startingNormals = shapeDefinition.GetStartingNormals();
    const std::vector<int>& startingFlags = shapeDefinition.GetStartingFlags();
    const size_t startingNormalCount = startingPositions.size();
    unsigned int index1 = shapeDefinition.GetRowStartIndex(0) / 4 * 12;
    unsigned int index2 = shapeDefinition.GetRowStartIndex(1) / 4 * 12;
    const unsigned int columnCount = shapeDefinition.GetNumColumns();
    unsigned int count = shapeDefinition.GetNumOddRows() * columnCount / 4;

    startingNormals.resize(startingNormalCount);

    ClothNormalsUpdater::CalcNormalsForStream(startingPositions, startingNormals, startingFlags, index1, index2, count, columnCount);

    const unsigned int rowCount = shapeDefinition.GetNumRows();

    if (rowCount > 2)
    {
        index1 = shapeDefinition.GetRowStartIndex(1) / 4 * 12;
        index2 = shapeDefinition.GetRowStartIndex(2) / 4 * 12;
        count = (shapeDefinition.GetNumEvenRows() - 1) * columnCount / 4;

        ClothNormalsUpdater::CalcNormalsForStream(startingPositions, startingNormals, startingFlags, index1, index2, count, columnCount);
    }

    std::vector<Vector3> startingNormals2;
    unsigned int index = 0;
    const size_t vertexCount = startingNormalCount / 3;

    startingNormals2.resize(vertexCount);

    for (size_t i = 0; i < startingNormalCount / 12; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingNormals2[i * 4 + j].x = startingNormals[index++];
        }
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingNormals2[i * 4 + j].y = startingNormals[index++];
        }
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingNormals2[i * 4 + j].z = startingNormals[index++];
        }
    }

    for (size_t i = 0; i < vertexCount; ++i)
    {
        startingNormals2[i].Normalize();
    }

    return startingNormals2;
}

CloakWorks::SheetMeshControlInstance::SheetMeshControlInstance(ShapeDefinition& shapeDefinition) : shapeDefinition(shapeDefinition)
{
}

const unsigned int CloakWorks::SheetMeshControlInstance::GetNumVerts() const
{
    return shapeDefinition.GetNumNodes();
}

const unsigned int CloakWorks::SheetMeshControlInstance::GetNumIndices() const
{
    const std::vector<int>& startingFlags = shapeDefinition.GetStartingFlags();
    const unsigned int rowCount = shapeDefinition.GetNumRows();
    const unsigned int columnCount = shapeDefinition.GetNumColumns();
    unsigned int indexCount = 0;

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
                indexCount += 6;
            }
        }
    }

    return indexCount;
}

void CloakWorks::SheetMeshControlInstance::FillTexCoordsBuffer(std::vector<Vector2>& textureCoordinates) const
{
    const unsigned int rowCount = shapeDefinition.GetNumRows();
    const unsigned int columnCount = shapeDefinition.GetNumColumns();

    textureCoordinates.resize(GetNumVerts());

    for (unsigned int row = 0; row < rowCount; ++row)
    {
        const unsigned int rowStartIndex = shapeDefinition.GetRowStartIndex(row);
        unsigned int textureCoordinateIndex = rowStartIndex;

        for (unsigned int column = 0; column < columnCount; ++column)
        {
            Vector2 textureCoordinates2;

            textureCoordinates2.x = column * (1.f / (columnCount - 1));
            textureCoordinates2.y = row * (1.f / (rowCount - 1));

            textureCoordinates[textureCoordinateIndex] = textureCoordinates2;

            ++textureCoordinateIndex;
        }
    }
}

std::vector<Vector3> CloakWorks::TubeShapeDefinition::GenerateStartingNormals(CloakWorks::ShapeDefinition& shapeDefinition)
{
    const std::vector<float>& startingPositions = shapeDefinition.GetStartingPositions();
    std::vector<float>& startingNormals = shapeDefinition.GetStartingNormals();
    const std::vector<int>& startingFlags = shapeDefinition.GetStartingFlags();
    const size_t startingNormalCount = startingPositions.size();
    unsigned int index1 = shapeDefinition.GetRowStartIndex(0) / 4 * 12;
    unsigned int index2 = shapeDefinition.GetRowStartIndex(1) / 4 * 12;
    const unsigned int columnCount = shapeDefinition.GetNumColumns();
    unsigned int count = shapeDefinition.GetNumOddRows() * columnCount / 4;

    startingNormals.resize(startingNormalCount);

    ClothNormalsUpdater::CalcNormalsForStream(startingPositions, startingNormals, startingFlags, index1, index2, count, columnCount);

    index1 = shapeDefinition.GetRowStartIndex(1) / 4 * 12;
    index2 = shapeDefinition.GetRowStartIndex(2) / 4 * 12;
    count = (shapeDefinition.GetNumEvenRows() - 1) * columnCount / 4;

    ClothNormalsUpdater::CalcNormalsForStream(startingPositions, startingNormals, startingFlags, index1, index2, count, columnCount);

    index1 = shapeDefinition.GetRowStartIndex(shapeDefinition.GetNumRows() - 1) / 4 * 12;
    index2 = shapeDefinition.GetRowStartIndex(0) / 4 * 12;
    count = columnCount / 4;

    ClothNormalsUpdater::CalcNormalsForStream(startingPositions, startingNormals, startingFlags, index1, index2, count, columnCount);

    std::vector<Vector3> startingNormals2;
    unsigned int index = 0;
    const size_t vertexCount = startingNormalCount / 3;

    startingNormals2.resize(vertexCount);

    for (size_t i = 0; i < startingNormalCount / 12; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingNormals2[i * 4 + j].x = startingNormals[index++];
        }
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingNormals2[i * 4 + j].y = startingNormals[index++];
        }
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingNormals2[i * 4 + j].z = startingNormals[index++];
        }
    }

    for (int i = 0; i < vertexCount; ++i)
    {
        startingNormals2[i].Normalize();
    }

    return startingNormals2;
}

std::vector<Vector3> CloakWorks::StrandShapeDefinition::GenerateStartingNormals(CloakWorks::ShapeDefinition& shapeDefinition)
{
    const std::vector<float>& startingPositions = shapeDefinition.GetStartingPositions();
    std::vector<float> startingNormals;
    const unsigned int rowCount = shapeDefinition.GetNumRows();
    const unsigned int columnCount = shapeDefinition.GetNumColumns();

    startingNormals.resize(startingPositions.size());

    if (rowCount != 1)
    {
        for (unsigned int row = 0; row < rowCount - 1; ++row)
        {
            for (unsigned int colulmn = 0; colulmn < columnCount; ++colulmn)
            {
                unsigned int index1 = (shapeDefinition.GetRowStartIndex(row) + colulmn) / 4 * 12;
                unsigned int index2 = (shapeDefinition.GetRowStartIndex(row + 1) + colulmn) / 4 * 12;
                const unsigned int index3 = (shapeDefinition.GetRowStartIndex(row) + colulmn) % 4;
                const unsigned int index4 = (shapeDefinition.GetRowStartIndex(row + 1) + colulmn) % 4;

                index1 += index3;
                index2 += index4;

                Vector3 vector1, vector2;
                Vector3 previousStartingNormal1, previousStartingNormal2;

                vector1.x = startingPositions[index1];
                vector1.y = startingPositions[index1 + 4];
                vector1.z = startingPositions[index1 + 8];

                vector2.x = startingPositions[index2];
                vector2.y = startingPositions[index2 + 4];
                vector2.z = startingPositions[index2 + 8];

                previousStartingNormal1.x = startingNormals[index1];
                previousStartingNormal1.y = startingNormals[index1 + 4];
                previousStartingNormal1.z = startingNormals[index1 + 8];

                previousStartingNormal2.x = startingNormals[index2];
                previousStartingNormal2.y = startingNormals[index2 + 4];
                previousStartingNormal2.z = startingNormals[index2 + 8];

                const Vector3 normal = (vector2 - vector1).Normalized();
                const Vector3 startingNormal1 = normal + previousStartingNormal1;
                const Vector3 startingNormal2 = normal + previousStartingNormal2;

                startingNormals[index1] = startingNormal1.x;
                startingNormals[index1 + 4] = startingNormal1.y;
                startingNormals[index1 + 8] = startingNormal1.z;

                startingNormals[index2] = startingNormal2.x;
                startingNormals[index2 + 4] = startingNormal2.y;
                startingNormals[index2 + 8] = startingNormal2.z;
            }
        }
    }

    const unsigned int nodeCount = shapeDefinition.GetNumNodes();

    for (unsigned int i = 0; i < nodeCount; ++i)
    {
        unsigned int index1 = i / 4 * 12;
        unsigned int index2 = i % 4;
        Vector3 startingNormal;

        index1 += index2;

        startingNormal.x = startingNormals[index1];
        startingNormal.y = startingNormals[index1 + 4];
        startingNormal.z = startingNormals[index1 + 8];

        startingNormal.Normalize();

        startingNormals[index1] = startingNormal.x;
        startingNormals[index1 + 4] = startingNormal.y;
        startingNormals[index1 + 8] = startingNormal.z;
    }

    std::vector<Vector3> startingNormals2;
    unsigned int index = 0;
    const size_t vertexCount = startingNormals.size() / 3;

    startingNormals2.resize(vertexCount);

    for (size_t i = 0; i < startingNormals.size() / 12; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingNormals2[i * 4 + j].x = startingNormals[index++];
        }
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingNormals2[i * 4 + j].y = startingNormals[index++];
        }
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingNormals2[i * 4 + j].z = startingNormals[index++];
        }
    }

    return startingNormals2;
}

CloakWorks::StrandMeshControlInstance::StrandMeshControlInstance(ShapeDefinition& shapeDefinition) : shapeDefinition(shapeDefinition)
{
}

const unsigned int CloakWorks::StrandMeshControlInstance::GetNumVerts() const
{
    return shapeDefinition.GetNumNodes() * 2;
}

const unsigned int CloakWorks::StrandMeshControlInstance::GetNumIndices() const
{
    const std::vector<int>& startingFlags = shapeDefinition.GetStartingFlags();
    const unsigned int rowCount = shapeDefinition.GetNumRows();
    const unsigned int columnCount = shapeDefinition.GetNumColumns();
    unsigned int indexCount = 0;

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
                indexCount += 6;
            }
        }
    }

    return indexCount;
}

void CloakWorks::StrandMeshControlInstance::FillTexCoordsBuffer(std::vector<Vector2>& textureCoordinates) const
{
    std::vector<float> textureCoordinates2;
    const unsigned int rowCount = shapeDefinition.GetNumRows();
    const unsigned int columnCount = shapeDefinition.GetNumColumns();

    textureCoordinates.resize(GetNumVerts());
    textureCoordinates2.resize(GetNumVerts() * 2);

    for (unsigned int column = 0; column < columnCount; ++column)
    {
        for (unsigned int row = 0; row < rowCount; ++row)
        {
            const unsigned int textureCoordinateIndex = 4 * (shapeDefinition.GetRowStartIndex(row) + column);

            textureCoordinates2[textureCoordinateIndex] = 0.f;
            textureCoordinates2[textureCoordinateIndex + 1] = static_cast<float>(row) / (rowCount - 1);
            textureCoordinates2[textureCoordinateIndex + 2] = 1.f;
            textureCoordinates2[textureCoordinateIndex + 3] = static_cast<float>(row) / (rowCount - 1);
        }
    }

    for (size_t i = 0; i < textureCoordinates2.size() / 2; ++i)
    {
        textureCoordinates[i] = Vector2(textureCoordinates2[i * 2], textureCoordinates2[i * 2 + 1]);
    }
}

CloakWorks::TubeMeshControlInstance::TubeMeshControlInstance(ShapeDefinition& shapeDefinition) : shapeDefinition(shapeDefinition)
{
}

const unsigned int CloakWorks::TubeMeshControlInstance::GetNumVerts() const
{
    return shapeDefinition.GetNumNodes() + shapeDefinition.GetNumColumns();
}

const unsigned int CloakWorks::TubeMeshControlInstance::GetNumIndices() const
{
    const std::vector<int>& startingFlags = shapeDefinition.GetStartingFlags();
    const unsigned int rowCount = shapeDefinition.GetNumRows();
    const unsigned int columnCount = shapeDefinition.GetNumColumns();
    unsigned int indexCount = 0;

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
            const unsigned int currentFlagIndex = currentRowStartIndex + column;
            const unsigned int nextFlagIndex = nextRowStartIndex + column;

            if ((startingFlags[currentFlagIndex] & 8) == 0
                && (startingFlags[currentFlagIndex + 1] & 8) == 0
                && (startingFlags[nextFlagIndex] & 8) == 0
                && (startingFlags[nextFlagIndex + 1] & 8) == 0)
            {
                indexCount += 6;
            }
        }
    }

    return indexCount;
}

void CloakWorks::TubeMeshControlInstance::FillTexCoordsBuffer(std::vector<Vector2>& textureCoordinates) const
{
    std::vector<float> textureCoordinates2;
    const unsigned int rowCount = shapeDefinition.GetNumRows();
    const unsigned int columnCount = shapeDefinition.GetNumColumns();

    textureCoordinates.resize(GetNumVerts());
    textureCoordinates2.resize(GetNumVerts() * 2);

    for (unsigned int row = 0; row <= rowCount; ++row)
    {
        unsigned int rowStartIndex;

        if (row == rowCount)
        {
            rowStartIndex = shapeDefinition.GetNumNodes();
        }
        else
        {
            rowStartIndex = shapeDefinition.GetRowStartIndex(row);
        }

        unsigned int textureCoordinateIndex = 2 * rowStartIndex;

        for (unsigned int column = 0; column < columnCount; ++column)
        {
            textureCoordinates[textureCoordinateIndex] = column * (1.f / (columnCount - 1));
            textureCoordinates[textureCoordinateIndex + 1] = row * (1.f / rowCount);

            textureCoordinateIndex += 2;
        }
    }

    for (size_t i = 0; i < textureCoordinates2.size() / 2; ++i)
    {
        textureCoordinates[i] = Vector2(textureCoordinates2[i * 2], textureCoordinates2[i * 2 + 1]);
    }
}
