#pragma once

#include <string>
#include <vector>

#include "CloakWorks/CloakWorks.h"

#include "IO/BinaryReader.h"

#undef GetClassName

class Cloth
{
public:
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

    class BoolNode : public Node
    {
    public:
        BoolNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const std::vector<bool>& GetPrimitives() const;
        void AddPrimitive(const bool value);

    private:
        std::vector<bool> primitives;
    };

    class ByteNode : public Node
    {
    public:
        ByteNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const std::vector<unsigned char>& GetPrimitives() const;
        void AddPrimitive(const unsigned char value);

    private:
        std::vector<unsigned char> primitives;
    };

    class CharNode : public Node
    {
    public:
        CharNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const std::vector<char>& GetPrimitives() const;
        void AddPrimitive(const char value);

    private:
        std::vector<char> primitives;
    };

    class UShortNode : public Node
    {
    public:
        UShortNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const std::vector<unsigned short>& GetPrimitives() const;
        void AddPrimitive(const unsigned short value);

    private:
        std::vector<unsigned short> primitives;
    };

    class ShortNode : public Node
    {
    public:
        ShortNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const std::vector<short>& GetPrimitives() const;
        void AddPrimitive(const short value);

    private:
        std::vector<short> primitives;
    };

    class UIntNode : public Node
    {
    public:
        UIntNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const std::vector<unsigned int>& GetPrimitives() const;
        void AddPrimitive(const unsigned int value);

    private:
        std::vector<unsigned int> primitives;
    };

    class IntNode : public Node
    {
    public:
        IntNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const std::vector<int>& GetPrimitives() const;
        void AddPrimitive(const int value);

    private:
        std::vector<int> primitives;
    };

    class FloatNode : public Node
    {
    public:
        FloatNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const std::vector<float>& GetPrimitives() const;
        void AddPrimitive(const float value);

    private:
        std::vector<float> primitives;
    };

    class StringNode : public Node
    {
    public:
        StringNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        const std::vector<std::string>& GetPrimitives() const;
        void AddPrimitive(const std::string& value);

    private:
        std::vector<std::string> primitives;
    };

    class ObjectNode : public Node
    {
    public:
        ObjectNode(const CloakWorks::Reflection::FieldType fieldType);
        ObjectNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className);
        ~ObjectNode();
        const std::vector<Node*>& GetChildren() const;
        void AddChild(Node* childNode);

    private:
        std::vector<Node*> children;
    };

    class ArrayNode : public Node
    {
    public:
        ArrayNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className, unsigned int arrayPrimitiveCount);
        ~ArrayNode();
        unsigned int GetArrayPrimitiveCount() const;
        const std::vector<Node*>& GetChildren() const;
        void AddChild(Node* childNode);

    private:
        unsigned int arrayPrimitiveCount;
        std::vector<Node*> children;
    };

    ~Cloth();
    void Deserialize(const std::string& filePath);
    void Deserialize(void* buffer, const unsigned int dataSize);
    void Deserialize(BinaryReader& binaryReader);
    void Deserialize(Node* node, BinaryReader& binaryReader, unsigned int& binaryNodeOffset, unsigned int& nextBinaryNodeOffset, unsigned int depth);
    static unsigned char GetBaseTypeSize(const CloakWorks::Reflection::FieldType fieldType);
    void SerializeToXML(std::string& xmlOutput);
    void SerializeToXML(Node* node, std::stringstream& stringStream, unsigned int depth);
    static std::string ConvertFieldTypeToString(const CloakWorks::Reflection::FieldType fieldType);

private:
    Node* shroudObject;
};
