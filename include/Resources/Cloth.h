#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <format>

#include "CloakWorks/CloakWorks.h"

#include "Resources/Resource.h"
#include "Math/Vector4.h"
#include "Math/Vector2.h"

class RenderMaterialInstance;

#undef GetClassName

class Cloth : public Resource
{
public:
    struct MappingInfo
    {
        Vector4 barycentricCoordinates;
        Vector4 positionOffset;
        Vector4 normalOffset;
        Vector4 tangentOffset;
    };

    struct Vertex
    {
        Vector3 position;
        Vector3 normal;
        Vector4 tangent;
        Vector3 bitangent;
        Vector2 textureCoordinates;
    };

    class Node
    {
    public:
        Node(const CloakWorks::Reflection::FieldType fieldType);
        Node(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const CloakWorks::Reflection::FieldType GetFieldType() const;
        const std::string GetName() const;
        const std::string GetClassName() const;
        void SetFieldType(const CloakWorks::Reflection::FieldType fieldType);
        void SetName(const std::string name);
        void SetClassName(const std::string className);

    private:
        CloakWorks::Reflection::FieldType fieldType;
        std::string name;
        std::string className;
    };

    template <typename T>
    class PrimitiveNode : public Node
    {
    public:
        PrimitiveNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
        {
        }

        const std::vector<T>& GetPrimitives() const
        {
            return primitives;
        }

        void AddPrimitive(const T value)
        {
            primitives.push_back(value);
        }

    private:
        std::vector<T> primitives;
    };

    class ObjectNode : public Node
    {
    public:
        ObjectNode(const CloakWorks::Reflection::FieldType fieldType);
        ObjectNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const std::vector<std::shared_ptr<Node>>& GetChildren() const;
        const std::shared_ptr<Node> GetChildByName(const std::string& name);
        const std::shared_ptr<Node> GetChildByClassName(const std::string& className);
        void AddChild(std::shared_ptr<Node> childNode);

    private:
        std::vector<std::shared_ptr<Node>> children;
    };

    class ArrayNode : public Node
    {
    public:
        ArrayNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className, unsigned int arrayPrimitiveCount);
        unsigned int GetArrayPrimitiveCount() const;
        const std::vector<std::shared_ptr<Node>>& GetChildren() const;
        void AddChild(std::shared_ptr<Node> childNode);

    private:
        unsigned int arrayPrimitiveCount;
        std::vector<std::shared_ptr<Node>> children;
    };

    void Deserialize() override;
    void Deserialize(std::shared_ptr<Node> node, BinaryReader& binaryReader, unsigned int depth);
    void Export(const std::string& outputPath, const std::string& exportOption) override;
    void GenerateVerticesAndIndices();
    std::shared_ptr<RenderMaterialInstance> FindMaterialReference();

    template <typename T>
    void DeserializePrimitives(BinaryReader& binaryReader, std::shared_ptr<Node>& childNode, const CloakWorks::BinaryNode& binaryNode, const std::string& name, const std::string& className)
    {
        childNode = std::make_shared<PrimitiveNode<T>>(binaryNode.primitiveType, name, className);

        if (binaryNode.dataOffset > 0)
        {
            for (unsigned int i = 0; i < binaryNode.primitiveCount; ++i)
            {
                std::static_pointer_cast<PrimitiveNode<T>>(childNode)->AddPrimitive(binaryReader.Read<T>());
            }
        }
    }

    template <>
    void DeserializePrimitives<std::string>(BinaryReader& binaryReader, std::shared_ptr<Node>& childNode, const CloakWorks::BinaryNode& binaryNode, const std::string& name, const std::string& className)
    {
        childNode = std::make_shared<PrimitiveNode<std::string>>(binaryNode.primitiveType, name, className);

        if (binaryNode.dataOffset > 0)
        {
            std::static_pointer_cast<PrimitiveNode<std::string>>(childNode)->AddPrimitive(binaryReader.ReadString(static_cast<size_t>(binaryNode.dataSize)));
        }
    }

    template <typename T>
    void DeserializeArrayPrimitives(BinaryReader& binaryReader, std::shared_ptr<Node> childNode, const CloakWorks::BinaryNode& binaryNode)
    {
        unsigned int arrayCount = binaryNode.dataSize / GetBaseTypeSize(binaryNode.arrayPrimitiveType);

        for (unsigned int i = 0; i < arrayCount; ++i)
        {
            std::shared_ptr<Node> node = std::make_shared<PrimitiveNode<T>>(binaryNode.arrayPrimitiveType, "", "");

            std::static_pointer_cast<PrimitiveNode<T>>(node)->AddPrimitive(binaryReader.Read<T>());
            std::static_pointer_cast<ArrayNode>(childNode)->AddChild(node);
        }
    }

    static unsigned char GetBaseTypeSize(const CloakWorks::Reflection::FieldType fieldType);
    void SerializeToXML(const std::string& outputFilePath);
    void SerializeToXML(std::shared_ptr<Node> node, std::stringstream& stringStream, unsigned int depth);

    template <typename T>
    void SerializePrimitivesToXML(std::shared_ptr<Node> node, std::stringstream& stringStream, unsigned int depth)
    {
        const std::vector<T>& primitives = std::static_pointer_cast<PrimitiveNode<T>>(node)->GetPrimitives();
        std::string nodeType = ConvertFieldTypeToString(node->GetFieldType());

        stringStream << std::format("<{} name=\"{}\" primitiveCount=\"{}\">", nodeType, node->GetName(), primitives.size());

        for (size_t i = 0; i < primitives.size(); ++i)
        {
            stringStream << primitives[i];

            if (i < primitives.size() - 1)
            {
                stringStream << " ";
            }
        }

        stringStream << std::format("</{}>", nodeType) << std::endl;
    }

    template <typename T>
    void SerializeArrayPrimitivesToXML(std::shared_ptr<Node> node, std::stringstream& stringStream, unsigned int depth)
    {
        const std::vector<std::shared_ptr<Node>> children = std::static_pointer_cast<ArrayNode>(node)->GetChildren();

        for (size_t i = 0; i < children.size(); ++i)
        {
            stringStream << std::static_pointer_cast<PrimitiveNode<T>>(children[i])->GetPrimitives()[0];

            if (i < children.size() - 1)
            {
                stringStream << " ";
            }
        }
    }

    static std::string ConvertFieldTypeToString(const CloakWorks::Reflection::FieldType fieldType);

private:
    void ExportMesh(const std::string& outputPath, const bool exportToOBJ);
    void ExportMeshToOBJ(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& simulationObjectName, const std::string& outputPath);
    void ExportMeshToGLB(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& simulationObjectName, const std::string& outputPath, const bool rotate);

    static std::vector<Vertex> GenerateVerticesForThickMesh(CloakWorks::ShapeDefinition& shapeDefinition, const std::vector<Vector3>& startingPositions, const std::vector<Vector3>& startingNormals, const std::shared_ptr<ObjectNode> thickMeshControlObject);
    static void GenerateVerticesAndIndicesForSheetMesh(CloakWorks::ShapeDefinition& shapeDefinition, const std::vector<Vector3>& startingPositions, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    static void GenerateVerticesAndIndicesForTubeMesh(CloakWorks::ShapeDefinition& shapeDefinition, const std::vector<Vector3>& startingPositions, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    static void GenerateVerticesAndIndicesForStrandMesh(CloakWorks::ShapeDefinition& shapeDefinition, const std::vector<Vector3>& startingPositions, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

    static std::vector<Vector4> CalculateTangents(CloakWorks::ShapeDefinition& shapeDefinition, const std::vector<Vector3>& startingPositions, std::vector<Vector3> startingNormals, const std::vector<unsigned int>& triangleIndices, const std::vector<Vector2>& textureCoordinates);
    static std::vector<Vector3> CalculateBitangents(std::vector<Vertex>& vertices);

    std::shared_ptr<Node> shroudObject;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};
