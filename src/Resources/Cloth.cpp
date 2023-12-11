#include <sstream>
#include <format>

#include "Resources/Cloth.h"

Cloth::Node::Node(const CloakWorks::Reflection::FieldType fieldType) : fieldType(fieldType)
{
}

Cloth::Node::Node(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : fieldType(fieldType), name(name), className(className)
{
}

const CloakWorks::Reflection::FieldType Cloth::Node::GetFieldType() const
{
    return fieldType;
}

const std::string Cloth::Node::GetName() const
{
    return name;
}

const std::string Cloth::Node::GetClassName() const
{
    return className;
}

void Cloth::Node::SetFieldType(const CloakWorks::Reflection::FieldType fieldType)
{
    this->fieldType = fieldType;
}

void Cloth::Node::SetName(const std::string name)
{
    this->name = name;
}

void Cloth::Node::SetClassName(const std::string className)
{
    this->className = className;
}

Cloth::BoolNode::BoolNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

const std::vector<bool>& Cloth::BoolNode::GetPrimitives() const
{
    return primitives;
}

void Cloth::BoolNode::AddPrimitive(const bool value)
{
    primitives.push_back(value);
}

Cloth::ByteNode::ByteNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

const std::vector<unsigned char>& Cloth::ByteNode::GetPrimitives() const
{
    return primitives;
}

void Cloth::ByteNode::AddPrimitive(const unsigned char value)
{
    primitives.push_back(value);
}

Cloth::CharNode::CharNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

const std::vector<char>& Cloth::CharNode::GetPrimitives() const
{
    return primitives;
}

void Cloth::CharNode::AddPrimitive(const char value)
{
    primitives.push_back(value);
}

Cloth::UShortNode::UShortNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

const std::vector<unsigned short>& Cloth::UShortNode::GetPrimitives() const
{
    return primitives;
}

void Cloth::UShortNode::AddPrimitive(const unsigned short value)
{
    primitives.push_back(value);
}

Cloth::ShortNode::ShortNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

const std::vector<short>& Cloth::ShortNode::GetPrimitives() const
{
    return primitives;
}

void Cloth::ShortNode::AddPrimitive(const short value)
{
    primitives.push_back(value);
}

Cloth::UIntNode::UIntNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

const std::vector<unsigned int>& Cloth::UIntNode::GetPrimitives() const
{
    return primitives;
}

void Cloth::UIntNode::AddPrimitive(const unsigned int value)
{
    primitives.push_back(value);
}

Cloth::IntNode::IntNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

const std::vector<int>& Cloth::IntNode::GetPrimitives() const
{
    return primitives;
}

void Cloth::IntNode::AddPrimitive(const int value)
{
    primitives.push_back(value);
}

Cloth::FloatNode::FloatNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

const std::vector<float>& Cloth::FloatNode::GetPrimitives() const
{
    return primitives;
}

void Cloth::FloatNode::AddPrimitive(const float value)
{
    primitives.push_back(value);
}

Cloth::StringNode::StringNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

const std::vector<std::string>& Cloth::StringNode::GetPrimitives() const
{
    return primitives;
}

void Cloth::StringNode::AddPrimitive(const std::string& value)
{
    primitives.push_back(value);
}

Cloth::ObjectNode::ObjectNode(const CloakWorks::Reflection::FieldType fieldType) : Node(fieldType)
{
}

Cloth::ObjectNode::ObjectNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

Cloth::ObjectNode::~ObjectNode()
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        delete children[i];
    }

    children.clear();
}

const std::vector<Cloth::Node*>& Cloth::ObjectNode::GetChildren() const
{
    return children;
}

void Cloth::ObjectNode::AddChild(Node* childNode)
{
    children.push_back(childNode);
}

Cloth::ArrayNode::ArrayNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className, unsigned int arrayPrimitiveCount) : Node(fieldType, name, className), arrayPrimitiveCount(arrayPrimitiveCount)
{
}

Cloth::ArrayNode::~ArrayNode()
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        delete children[i];
    }

    children.clear();
}

unsigned int Cloth::ArrayNode::GetArrayPrimitiveCount() const
{
    return arrayPrimitiveCount;
}

const std::vector<Cloth::Node*>& Cloth::ArrayNode::GetChildren() const
{
    return children;
}

void Cloth::ArrayNode::AddChild(Node* childNode)
{
    children.push_back(childNode);
}

Cloth::~Cloth()
{
    delete shroudObject;
}

void Cloth::Deserialize(const std::string& filePath)
{
    BinaryReader binaryReader = BinaryReader(filePath);

    Deserialize(binaryReader);
}

void Cloth::Deserialize(void* buffer, const unsigned int dataSize)
{
    BinaryReader binaryReader = BinaryReader(buffer, dataSize);

    Deserialize(binaryReader);
}

void Cloth::Deserialize(BinaryReader& binaryReader)
{
    binaryReader.Seek(0x24, SeekOrigin::Begin);

    unsigned int binaryNodeOffset = binaryReader.Read<unsigned int>();
    unsigned int outerBinaryNodeOffset = 0;

    binaryReader.Seek(binaryNodeOffset - 4, SeekOrigin::Current);

    shroudObject = new ObjectNode(CloakWorks::Reflection::FieldType::kFieldType_Invalid);

    Deserialize(shroudObject, binaryReader, binaryNodeOffset, outerBinaryNodeOffset, 0);
}

void Cloth::Deserialize(Node* node, BinaryReader& binaryReader, unsigned int& binaryNodeOffset, unsigned int& nextBinaryNodeOffset, unsigned int depth)
{
    while (binaryReader.GetPosition() < binaryReader.GetSize())
    {
        Node* childNode = nullptr;
        size_t currentPosition = binaryReader.GetPosition();
        CloakWorks::BinaryNode binaryNode = binaryReader.Read<CloakWorks::BinaryNode>();
        std::string name;
        std::string className;

        if (binaryNode.nameOffset > 0)
        {
            binaryReader.Seek(currentPosition + binaryNode.nameOffset, SeekOrigin::Begin);

            name = binaryReader.ReadString(static_cast<size_t>(binaryNode.nameLength));

            if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Invalid)
            {
                node->SetName(name);
            }
        }

        if (binaryNode.classNameOffset > 0)
        {
            binaryReader.Seek(currentPosition + binaryNode.classNameOffset + 8, SeekOrigin::Begin);

            className = binaryReader.ReadString(static_cast<size_t>(binaryNode.classNameLength));

            if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Invalid)
            {
                node->SetClassName(className);
            }
        }

        if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Invalid)
        {
            node->SetFieldType(binaryNode.primitiveType);
        }

        if (binaryNode.primitiveType == CloakWorks::Reflection::FieldType::kFieldType_Object)
        {
            if (binaryNode.nextBinaryNodeOffset > 0)
            {
                nextBinaryNodeOffset = static_cast<unsigned int>(currentPosition) + offsetof(CloakWorks::BinaryNode, nextBinaryNodeOffset) + binaryNode.nextBinaryNodeOffset;
            }

            binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, childBinaryNodeOffset) + binaryNode.childBinaryNodeOffset, SeekOrigin::Begin);

            if (depth != 0)
            {
                childNode = new ObjectNode(binaryNode.primitiveType, name, className);

                Deserialize(childNode, binaryReader, binaryNodeOffset, nextBinaryNodeOffset, depth + 1);
            }
            else
            {
                Deserialize(node, binaryReader, binaryNodeOffset, nextBinaryNodeOffset, depth + 1);
            }

            if (binaryNode.nextBinaryNodeOffset == 0)
            {
                if (childNode)
                {
                    if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
                    {
                        static_cast<ObjectNode*>(node)->AddChild(childNode);
                    }
                    else if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Array)
                    {
                        static_cast<ArrayNode*>(node)->AddChild(childNode);
                    }
                }

                break;
            }

            binaryNodeOffset = binaryNode.nextBinaryNodeOffset;

            binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, nextBinaryNodeOffset) + binaryNodeOffset, SeekOrigin::Begin);
        }
        else if (binaryNode.primitiveType == CloakWorks::Reflection::FieldType::kFieldType_Array)
        {
            childNode = new ArrayNode(binaryNode.primitiveType, name, className, binaryNode.arrayPrimitiveCount);

            if (binaryNode.arrayPrimitiveType == CloakWorks::Reflection::FieldType::kFieldType_Object)
            {
                if (binaryNode.nextBinaryNodeOffset > 0)
                {
                    nextBinaryNodeOffset = static_cast<unsigned int>(currentPosition) + 0x10 + binaryNode.nextBinaryNodeOffset;
                }

                binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, childBinaryNodeOffset) + binaryNode.childBinaryNodeOffset, SeekOrigin::Begin);

                Deserialize(childNode, binaryReader, binaryNodeOffset, nextBinaryNodeOffset, depth + 1);

                if (binaryNode.nextBinaryNodeOffset == 0)
                {
                    if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
                    {
                        static_cast<ObjectNode*>(node)->AddChild(childNode);
                    }
                    else if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Array)
                    {
                        static_cast<ArrayNode*>(node)->AddChild(childNode);
                    }

                    break;
                }

                binaryNodeOffset = binaryNode.nextBinaryNodeOffset;

                binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, nextBinaryNodeOffset) + binaryNodeOffset, SeekOrigin::Begin);
            }
            else
            {
                binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, dataOffset) + binaryNode.dataOffset, SeekOrigin::Begin);

                unsigned int arrayCount = binaryNode.dataSize / GetBaseTypeSize(binaryNode.arrayPrimitiveType);

                switch (binaryNode.arrayPrimitiveType)
                {
                    case CloakWorks::Reflection::FieldType::kFieldType_bool:
                    {
                        for (unsigned int i = 0; i < arrayCount; ++i)
                        {
                            BoolNode* boolNode = new BoolNode(binaryNode.arrayPrimitiveType, "", "");

                            boolNode->AddPrimitive(binaryReader.Read<bool>());

                            static_cast<ArrayNode*>(childNode)->AddChild(boolNode);
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_byte:
                    {
                        for (unsigned int i = 0; i < arrayCount; ++i)
                        {
                            ByteNode* byteNode = new ByteNode(binaryNode.arrayPrimitiveType, "", "");

                            byteNode->AddPrimitive(binaryReader.Read<unsigned char>());

                            static_cast<ArrayNode*>(childNode)->AddChild(byteNode);
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_char:
                    {
                        for (unsigned int i = 0; i < arrayCount; ++i)
                        {
                            CharNode* charNode = new CharNode(binaryNode.arrayPrimitiveType, "", "");

                            charNode->AddPrimitive(binaryReader.Read<char>());

                            static_cast<ArrayNode*>(childNode)->AddChild(charNode);
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_uint16:
                    {
                        for (unsigned int i = 0; i < arrayCount; ++i)
                        {
                            UShortNode* uShortNode = new UShortNode(binaryNode.arrayPrimitiveType, "", "");

                            uShortNode->AddPrimitive(binaryReader.Read<unsigned short>());

                            static_cast<ArrayNode*>(childNode)->AddChild(uShortNode);
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_int16:
                    {
                        for (unsigned int i = 0; i < arrayCount; ++i)
                        {
                            ShortNode* shortNode = new ShortNode(binaryNode.arrayPrimitiveType, "", "");

                            shortNode->AddPrimitive(binaryReader.Read<short>());

                            static_cast<ArrayNode*>(childNode)->AddChild(shortNode);
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_uint32:
                    {
                        for (unsigned int i = 0; i < arrayCount; ++i)
                        {
                            UIntNode* uIntNode = new UIntNode(binaryNode.arrayPrimitiveType, "", "");

                            uIntNode->AddPrimitive(binaryReader.Read<unsigned int>());

                            static_cast<ArrayNode*>(childNode)->AddChild(uIntNode);
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_int32:
                    {
                        for (unsigned int i = 0; i < arrayCount; ++i)
                        {
                            IntNode* intNode = new IntNode(binaryNode.arrayPrimitiveType, "", "");

                            intNode->AddPrimitive(binaryReader.Read<int>());

                            static_cast<ArrayNode*>(childNode)->AddChild(intNode);
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_float:
                    {
                        for (unsigned int i = 0; i < arrayCount; ++i)
                        {
                            FloatNode* floatNode = new FloatNode(binaryNode.arrayPrimitiveType, "", "");

                            floatNode->AddPrimitive(binaryReader.Read<float>());

                            static_cast<ArrayNode*>(childNode)->AddChild(floatNode);
                        }

                        break;
                    }
                }

                if (binaryReader.GetPosition() == nextBinaryNodeOffset || nextBinaryNodeOffset == 0)
                {
                    if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
                    {
                        static_cast<ObjectNode*>(node)->AddChild(childNode);
                    }
                    else if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Array)
                    {
                        static_cast<ArrayNode*>(node)->AddChild(childNode);
                    }

                    break;
                }
            }
        }
        else
        {
            binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, dataOffset) + binaryNode.dataOffset, SeekOrigin::Begin);

            switch (binaryNode.primitiveType)
            {
                case CloakWorks::Reflection::FieldType::kFieldType_bool:
                {
                    childNode = new BoolNode(binaryNode.primitiveType, name, className);

                    if (binaryNode.dataOffset > 0)
                    {
                        for (unsigned int i = 0; i < binaryNode.primitiveCount; ++i)
                        {
                            static_cast<BoolNode*>(childNode)->AddPrimitive(binaryReader.Read<bool>());
                        }
                    }

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_byte:
                {
                    childNode = new ByteNode(binaryNode.primitiveType, name, className);

                    if (binaryNode.dataOffset > 0)
                    {
                        for (unsigned int i = 0; i < binaryNode.primitiveCount; ++i)
                        {
                            static_cast<ByteNode*>(childNode)->AddPrimitive(binaryReader.Read<unsigned char>());
                        }
                    }

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_char:
                {
                    childNode = new CharNode(binaryNode.primitiveType, name, className);

                    if (binaryNode.dataOffset > 0)
                    {
                        for (unsigned int i = 0; i < binaryNode.primitiveCount; ++i)
                        {
                            static_cast<CharNode*>(childNode)->AddPrimitive(binaryReader.Read<char>());
                        }
                    }

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_uint16:
                {
                    childNode = new UShortNode(binaryNode.primitiveType, name, className);

                    if (binaryNode.dataOffset > 0)
                    {
                        for (unsigned int i = 0; i < binaryNode.primitiveCount; ++i)
                        {
                            static_cast<UShortNode*>(childNode)->AddPrimitive(binaryReader.Read<unsigned short>());
                        }
                    }

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_int16:
                {
                    childNode = new ShortNode(binaryNode.primitiveType, name, className);

                    if (binaryNode.dataOffset > 0)
                    {
                        for (unsigned int i = 0; i < binaryNode.primitiveCount; ++i)
                        {
                            static_cast<ShortNode*>(childNode)->AddPrimitive(binaryReader.Read<short>());
                        }
                    }

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_uint32:
                {
                    childNode = new UIntNode(binaryNode.primitiveType, name, className);

                    if (binaryNode.dataOffset > 0)
                    {
                        for (unsigned int i = 0; i < binaryNode.primitiveCount; ++i)
                        {
                            static_cast<UIntNode*>(childNode)->AddPrimitive(binaryReader.Read<unsigned int>());
                        }
                    }

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_int32:
                {
                    childNode = new IntNode(binaryNode.primitiveType, name, className);

                    if (binaryNode.dataOffset > 0)
                    {
                        for (unsigned int i = 0; i < binaryNode.primitiveCount; ++i)
                        {
                            static_cast<IntNode*>(childNode)->AddPrimitive(binaryReader.Read<int>());
                        }
                    }

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_float:
                {
                    childNode = new FloatNode(binaryNode.primitiveType, name, className);

                    if (binaryNode.dataOffset > 0)
                    {
                        for (unsigned int i = 0; i < binaryNode.primitiveCount; ++i)
                        {
                            static_cast<FloatNode*>(childNode)->AddPrimitive(binaryReader.Read<float>());
                        }
                    }

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_string:
                {
                    childNode = new StringNode(binaryNode.primitiveType, name, className);

                    if (binaryNode.dataOffset > 0)
                    {
                        for (unsigned int i = 0; i < binaryNode.primitiveCount; ++i)
                        {
                            static_cast<StringNode*>(childNode)->AddPrimitive(binaryReader.ReadString(static_cast<size_t>(binaryNode.dataSize)));
                        }
                    }

                    break;
                }
            }

            if (binaryNode.nextBinaryNodeOffset == 0)
            {
                if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
                {
                    static_cast<ObjectNode*>(node)->AddChild(childNode);
                }
                else if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Array)
                {
                    static_cast<ArrayNode*>(node)->AddChild(childNode);
                }

                break;
            }

            binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, nextBinaryNodeOffset) + binaryNode.nextBinaryNodeOffset, SeekOrigin::Begin);
        }

        if (childNode)
        {
            if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
            {
                static_cast<ObjectNode*>(node)->AddChild(childNode);
            }
            else if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Array)
            {
                static_cast<ArrayNode*>(node)->AddChild(childNode);
            }
        }
    }
}

unsigned char Cloth::GetBaseTypeSize(const CloakWorks::Reflection::FieldType fieldType)
{
    switch (fieldType)
    {
        case CloakWorks::Reflection::FieldType::kFieldType_bool:
        case CloakWorks::Reflection::FieldType::kFieldType_byte:
        case CloakWorks::Reflection::FieldType::kFieldType_char:
            return 1;
        case CloakWorks::Reflection::FieldType::kFieldType_uint16:
        case CloakWorks::Reflection::FieldType::kFieldType_int16:
            return 2;
        case CloakWorks::Reflection::FieldType::kFieldType_uint32:
        case CloakWorks::Reflection::FieldType::kFieldType_int32:
        case CloakWorks::Reflection::FieldType::kFieldType_float:
            return 4;
        default:
            return 0;
    }
}

void Cloth::SerializeToXML(std::string& xmlOutput)
{
    std::stringstream stringStream;

    stringStream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;

    SerializeToXML(shroudObject, stringStream, 0);

    xmlOutput = stringStream.str();
}

void Cloth::SerializeToXML(Node* node, std::stringstream& stringStream, unsigned int depth)
{
    CloakWorks::Reflection::FieldType fieldType = node->GetFieldType();
    std::string name = node->GetName();
    std::string className = node->GetClassName();

    if (fieldType == CloakWorks::Reflection::FieldType::kFieldType_string &&
        static_cast<StringNode*>(node)->GetPrimitives().size() == 0)
    {
        return;
    }

    for (unsigned int i = 0; i < depth; ++i)
    {
        stringStream << "\t";
    }

    switch (fieldType)
    {
        case CloakWorks::Reflection::FieldType::kFieldType_bool:
        {
            BoolNode* boolNode = static_cast<BoolNode*>(node);
            const std::vector<bool> primitives = boolNode->GetPrimitives();

            stringStream << std::format("<BOOL name=\"{}\" primitiveCount=\"{}\">", name, boolNode->GetPrimitives().size());

            for (size_t i = 0; i < primitives.size(); ++i)
            {
                stringStream << primitives[i];

                if (i < primitives.size() - 1)
                {
                    stringStream << " ";
                }
            }

            stringStream << "</BOOL>" << std::endl;

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_byte:
        {
            ByteNode* byteNode = static_cast<ByteNode*>(node);
            const std::vector<unsigned char> primitives = byteNode->GetPrimitives();

            stringStream << std::format("<BYTE name=\"{}\" primitiveCount=\"{}\">", name, byteNode->GetPrimitives().size());

            for (size_t i = 0; i < primitives.size(); ++i)
            {
                stringStream << primitives[i];

                if (i < primitives.size() - 1)
                {
                    stringStream << " ";
                }
            }

            stringStream << "</BYTE>" << std::endl;

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_char:
        {
            CharNode* charNode = static_cast<CharNode*>(node);
            const std::vector<char> primitives = charNode->GetPrimitives();

            stringStream << std::format("<CHAR name=\"{}\" primitiveCount=\"{}\">", name, charNode->GetPrimitives().size());

            for (size_t i = 0; i < primitives.size(); ++i)
            {
                stringStream << primitives[i];

                if (i < primitives.size() - 1)
                {
                    stringStream << " ";
                }
            }

            stringStream << "</CHAR>" << std::endl;

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_uint16:
        {
            UShortNode* uShortNode = static_cast<UShortNode*>(node);
            const std::vector<unsigned short> primitives = uShortNode->GetPrimitives();

            stringStream << std::format("<UINT16 name=\"{}\" primitiveCount=\"{}\">", name, uShortNode->GetPrimitives().size());

            for (size_t i = 0; i < primitives.size(); ++i)
            {
                stringStream << primitives[i];

                if (i < primitives.size() - 1)
                {
                    stringStream << " ";
                }
            }

            stringStream << "</UINT16>" << std::endl;

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_int16:
        {
            ShortNode* shortNode = static_cast<ShortNode*>(node);
            const std::vector<short> primitives = shortNode->GetPrimitives();

            stringStream << std::format("<INT16 name=\"{}\" primitiveCount=\"{}\">", name, shortNode->GetPrimitives().size());

            for (size_t i = 0; i < primitives.size(); ++i)
            {
                stringStream << primitives[i];

                if (i < primitives.size() - 1)
                {
                    stringStream << " ";
                }
            }

            stringStream << "</INT16>" << std::endl;

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_uint32:
        {
            UIntNode* uIntNode = static_cast<UIntNode*>(node);
            const std::vector<unsigned int> primitives = uIntNode->GetPrimitives();

            stringStream << std::format("<UINT32 name=\"{}\" primitiveCount=\"{}\">", name, uIntNode->GetPrimitives().size());

            for (size_t i = 0; i < primitives.size(); ++i)
            {
                stringStream << primitives[i];

                if (i < primitives.size() - 1)
                {
                    stringStream << " ";
                }
            }

            stringStream << "</UINT32>" << std::endl;

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_int32:
        {
            IntNode* intNode = static_cast<IntNode*>(node);
            const std::vector<int> primitives = intNode->GetPrimitives();

            stringStream << std::format("<INT32 name=\"{}\" primitiveCount=\"{}\">", name, intNode->GetPrimitives().size());

            for (size_t i = 0; i < primitives.size(); ++i)
            {
                stringStream << primitives[i];

                if (i < primitives.size() - 1)
                {
                    stringStream << " ";
                }
            }

            stringStream << "</INT32>" << std::endl;

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_float:
        {
            FloatNode* floatNode = static_cast<FloatNode*>(node);
            const std::vector<float> primitives = floatNode->GetPrimitives();

            stringStream << std::format("<FLOAT name=\"{}\" primitiveCount=\"{}\">", name, floatNode->GetPrimitives().size());

            for (size_t i = 0; i < primitives.size(); ++i)
            {
                stringStream << primitives[i];

                if (i < primitives.size() - 1)
                {
                    stringStream << " ";
                }
            }

            stringStream << "</FLOAT>" << std::endl;

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_string:
        {
            StringNode* stringNode = static_cast<StringNode*>(node);
            const std::vector<std::string> primitives = stringNode->GetPrimitives();

            stringStream << std::format("<STRING name=\"{}\" primitiveCount=\"{}\">", name, stringNode->GetPrimitives().size());

            for (size_t i = 0; i < primitives.size(); ++i)
            {
                stringStream << primitives[i];

                if (i < primitives.size() - 1)
                {
                    stringStream << " ";
                }
            }

            stringStream << "</STRING>" << std::endl;

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_Object:
        {
            ObjectNode* objectNode = static_cast<ObjectNode*>(node);
            const std::vector<Node*> children = objectNode->GetChildren();

            stringStream << std::format("<OBJECT", name, className);

            if (name.length() > 0)
            {
                stringStream << std::format(" name=\"{}\"", name);
            }

            if (className.length() > 0)
            {
                stringStream << std::format(" className=\"{}\"", className);
            }

            stringStream << ">" << std::endl;

            for (size_t i = 0; i < children.size(); ++i)
            {
                SerializeToXML(children[i], stringStream, depth + 1);
            }

            for (unsigned int i = 0; i < depth; ++i)
            {
                stringStream << "\t";
            }

            stringStream << "</OBJECT>" << std::endl;

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_Array:
        {
            ArrayNode* arrayNode = static_cast<ArrayNode*>(node);
            const std::vector<Node*> children = arrayNode->GetChildren();
            std::string arrayType = ConvertFieldTypeToString(children[0]->GetFieldType());
            unsigned int arrayPrimitiveCount = arrayNode->GetArrayPrimitiveCount();

            stringStream << std::format("<ARRAY name=\"{}\" array_type=\"{}\" array_primitiveCount=\"{}\" array_count=\"{}\">", name, arrayType, arrayPrimitiveCount, children.size());

            if (children[0]->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
            {
                stringStream << std::endl;

                for (size_t i = 0; i < children.size(); ++i)
                {
                    SerializeToXML(children[i], stringStream, depth + 1);
                }

                for (unsigned int i = 0; i < depth; ++i)
                {
                    stringStream << "\t";
                }
            }
            else
            {
                switch (children[0]->GetFieldType())
                {
                    case CloakWorks::Reflection::FieldType::kFieldType_bool:
                    {
                        for (size_t i = 0; i < children.size(); ++i)
                        {
                            stringStream << static_cast<BoolNode*>(children[i])->GetPrimitives()[0];

                            if (i < children.size() - 1)
                            {
                                stringStream << " ";
                            }
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_byte:
                    {
                        for (size_t i = 0; i < children.size(); ++i)
                        {
                            stringStream << static_cast<ByteNode*>(children[i])->GetPrimitives()[0];

                            if (i < children.size() - 1)
                            {
                                stringStream << " ";
                            }
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_char:
                    {
                        for (size_t i = 0; i < children.size(); ++i)
                        {
                            stringStream << static_cast<CharNode*>(children[i])->GetPrimitives()[0];

                            if (i < children.size() - 1)
                            {
                                stringStream << " ";
                            }
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_uint16:
                    {
                        for (size_t i = 0; i < children.size(); ++i)
                        {
                            stringStream << static_cast<UShortNode*>(children[i])->GetPrimitives()[0];

                            if (i < children.size() - 1)
                            {
                                stringStream << " ";
                            }
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_int16:
                    {
                        for (size_t i = 0; i < children.size(); ++i)
                        {
                            stringStream << static_cast<ShortNode*>(children[i])->GetPrimitives()[0];

                            if (i < children.size() - 1)
                            {
                                stringStream << " ";
                            }
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_uint32:
                    {
                        for (size_t i = 0; i < children.size(); ++i)
                        {
                            stringStream << static_cast<UIntNode*>(children[i])->GetPrimitives()[0];

                            if (i < children.size() - 1)
                            {
                                stringStream << " ";
                            }
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_int32:
                    {
                        for (size_t i = 0; i < children.size(); ++i)
                        {
                            stringStream << static_cast<IntNode*>(children[i])->GetPrimitives()[0];

                            if (i < children.size() - 1)
                            {
                                stringStream << " ";
                            }
                        }

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_float:
                    {
                        for (size_t i = 0; i < children.size(); ++i)
                        {
                            stringStream << static_cast<FloatNode*>(children[i])->GetPrimitives()[0];

                            if (i < children.size() - 1)
                            {
                                stringStream << " ";
                            }
                        }

                        break;
                    }
                }
            }

            stringStream << "</ARRAY>" << std::endl;

            break;
        }
    }
}

std::string Cloth::ConvertFieldTypeToString(const CloakWorks::Reflection::FieldType fieldType)
{
    switch (fieldType)
    {
        case CloakWorks::Reflection::FieldType::kFieldType_bool:
            return "BOOL";
        case CloakWorks::Reflection::FieldType::kFieldType_byte:
            return "BYTE";
        case CloakWorks::Reflection::FieldType::kFieldType_char:
            return "CHAR";
        case CloakWorks::Reflection::FieldType::kFieldType_uint16:
            return "UINT16";
        case CloakWorks::Reflection::FieldType::kFieldType_int16:
            return "INT16";
        case CloakWorks::Reflection::FieldType::kFieldType_uint32:
            return "UINT32";
        case CloakWorks::Reflection::FieldType::kFieldType_int32:
            return "INT32";
        case CloakWorks::Reflection::FieldType::kFieldType_float:
            return "FLOAT";
        case CloakWorks::Reflection::FieldType::kFieldType_string:
            return "STRING";
        case CloakWorks::Reflection::FieldType::kFieldType_Object:
            return "OBJECT";
        case CloakWorks::Reflection::FieldType::kFieldType_Array:
            return "ARRAY";
        default:
            return "";
    }
}
