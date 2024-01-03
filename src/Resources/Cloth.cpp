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

Cloth::ObjectNode::ObjectNode(const CloakWorks::Reflection::FieldType fieldType) : Node(fieldType)
{
}

Cloth::ObjectNode::ObjectNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className) : Node(fieldType, name, className)
{
}

const std::vector<std::shared_ptr<Cloth::Node>>& Cloth::ObjectNode::GetChildren() const
{
    return children;
}

void Cloth::ObjectNode::AddChild(std::shared_ptr<Node> childNode)
{
    children.push_back(childNode);
}

Cloth::ArrayNode::ArrayNode(const CloakWorks::Reflection::FieldType fieldType, const std::string& name, const std::string& className, unsigned int arrayPrimitiveCount) : Node(fieldType, name, className), arrayPrimitiveCount(arrayPrimitiveCount)
{
}

unsigned int Cloth::ArrayNode::GetArrayPrimitiveCount() const
{
    return arrayPrimitiveCount;
}

const std::vector<std::shared_ptr<Cloth::Node>>& Cloth::ArrayNode::GetChildren() const
{
    return children;
}

void Cloth::ArrayNode::AddChild(std::shared_ptr<Node> childNode)
{
    children.push_back(childNode);
}

void Cloth::Deserialize()
{
    BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());

    binaryReader.Seek(0x24, SeekOrigin::Begin);

    unsigned int binaryNodeOffset = binaryReader.Read<unsigned int>();

    binaryReader.Seek(binaryNodeOffset - 4, SeekOrigin::Current);

    shroudObject = std::make_shared<ObjectNode>(CloakWorks::Reflection::FieldType::kFieldType_Invalid);

    Deserialize(shroudObject, binaryReader, 0);
}

void Cloth::Deserialize(std::shared_ptr<Node> node, BinaryReader& binaryReader, unsigned int depth)
{
    while (binaryReader.GetPosition() < binaryReader.GetSize())
    {
        std::shared_ptr<Node> childNode;
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
            binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, childBinaryNodeOffset) + binaryNode.childBinaryNodeOffset, SeekOrigin::Begin);

            if (depth != 0)
            {
                childNode = std::make_shared<ObjectNode>(binaryNode.primitiveType, name, className);

                Deserialize(childNode, binaryReader, depth + 1);
            }
            else
            {
                Deserialize(node, binaryReader, depth + 1);
            }

            if (binaryNode.nextBinaryNodeOffset == 0)
            {
                if (childNode)
                {
                    if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
                    {
                        std::static_pointer_cast<ObjectNode>(node)->AddChild(childNode);
                    }
                    else if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Array)
                    {
                        std::static_pointer_cast<ArrayNode>(node)->AddChild(childNode);
                    }
                }

                break;
            }

            binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, nextBinaryNodeOffset) + binaryNode.nextBinaryNodeOffset, SeekOrigin::Begin);
        }
        else if (binaryNode.primitiveType == CloakWorks::Reflection::FieldType::kFieldType_Array)
        {
            childNode = std::make_shared<ArrayNode>(binaryNode.primitiveType, name, className, binaryNode.arrayPrimitiveCount);

            if (binaryNode.arrayPrimitiveType == CloakWorks::Reflection::FieldType::kFieldType_Object)
            {
                binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, childBinaryNodeOffset) + binaryNode.childBinaryNodeOffset, SeekOrigin::Begin);

                Deserialize(childNode, binaryReader, depth + 1);

                if (binaryNode.nextBinaryNodeOffset == 0)
                {
                    if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
                    {
                        std::static_pointer_cast<ObjectNode>(node)->AddChild(childNode);
                    }
                    else if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Array)
                    {
                        std::static_pointer_cast<ArrayNode>(node)->AddChild(childNode);
                    }

                    break;
                }

                binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, nextBinaryNodeOffset) + binaryNode.nextBinaryNodeOffset, SeekOrigin::Begin);
            }
            else
            {
                binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, dataOffset) + binaryNode.dataOffset, SeekOrigin::Begin);

                switch (binaryNode.arrayPrimitiveType)
                {
                    case CloakWorks::Reflection::FieldType::kFieldType_bool:
                    {
                        DeserializeArrayPrimitives<bool>(binaryReader, childNode, binaryNode);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_byte:
                    {
                        DeserializeArrayPrimitives<unsigned char>(binaryReader, childNode, binaryNode);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_char:
                    {
                        DeserializeArrayPrimitives<char>(binaryReader, childNode, binaryNode);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_uint16:
                    {
                        DeserializeArrayPrimitives<unsigned short>(binaryReader, childNode, binaryNode);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_int16:
                    {
                        DeserializeArrayPrimitives<short>(binaryReader, childNode, binaryNode);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_uint32:
                    {
                        DeserializeArrayPrimitives<unsigned int>(binaryReader, childNode, binaryNode);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_int32:
                    {
                        DeserializeArrayPrimitives<int>(binaryReader, childNode, binaryNode);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_float:
                    {
                        DeserializeArrayPrimitives<float>(binaryReader, childNode, binaryNode);

                        break;
                    }
                }

                if (binaryNode.nextBinaryNodeOffset == 0)
                {
                    if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
                    {
                        std::static_pointer_cast<ObjectNode>(node)->AddChild(childNode);
                    }
                    else if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Array)
                    {
                        std::static_pointer_cast<ArrayNode>(node)->AddChild(childNode);
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
                    DeserializePrimitives<bool>(binaryReader, childNode, binaryNode, name, className);

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_byte:
                {
                    DeserializePrimitives<unsigned char>(binaryReader, childNode, binaryNode, name, className);

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_char:
                {
                    DeserializePrimitives<char>(binaryReader, childNode, binaryNode, name, className);

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_uint16:
                {
                    DeserializePrimitives<unsigned short>(binaryReader, childNode, binaryNode, name, className);

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_int16:
                {
                    DeserializePrimitives<short>(binaryReader, childNode, binaryNode, name, className);

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_uint32:
                {
                    DeserializePrimitives<unsigned int>(binaryReader, childNode, binaryNode, name, className);

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_int32:
                {
                    DeserializePrimitives<int>(binaryReader, childNode, binaryNode, name, className);

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_float:
                {
                    DeserializePrimitives<float>(binaryReader, childNode, binaryNode, name, className);

                    break;
                }
                case CloakWorks::Reflection::FieldType::kFieldType_string:
                {
                    DeserializePrimitives<std::string>(binaryReader, childNode, binaryNode, name, className);

                    break;
                }
            }

            if (binaryNode.nextBinaryNodeOffset == 0)
            {
                if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
                {
                    std::static_pointer_cast<ObjectNode>(node)->AddChild(childNode);
                }
                else if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Array)
                {
                    std::static_pointer_cast<ArrayNode>(node)->AddChild(childNode);
                }

                break;
            }

            binaryReader.Seek(currentPosition + offsetof(CloakWorks::BinaryNode, nextBinaryNodeOffset) + binaryNode.nextBinaryNodeOffset, SeekOrigin::Begin);
        }

        if (childNode)
        {
            if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Object)
            {
                std::static_pointer_cast<ObjectNode>(node)->AddChild(childNode);
            }
            else if (node->GetFieldType() == CloakWorks::Reflection::FieldType::kFieldType_Array)
            {
                std::static_pointer_cast<ArrayNode>(node)->AddChild(childNode);
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

void Cloth::SerializeToXML(const std::string& filePath)
{
    std::stringstream stringStream;

    stringStream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;

    SerializeToXML(shroudObject, stringStream, 0);

    std::string xmlOutput = stringStream.str();
    std::ofstream ofstream = std::ofstream(filePath);

    ofstream << xmlOutput;

    ofstream.close();
}

void Cloth::SerializeToXML(std::shared_ptr<Node> node, std::stringstream& stringStream, unsigned int depth)
{
    CloakWorks::Reflection::FieldType fieldType = node->GetFieldType();
    std::string name = node->GetName();
    std::string className = node->GetClassName();

    if (fieldType == CloakWorks::Reflection::FieldType::kFieldType_string &&
        std::static_pointer_cast<PrimitiveNode<std::string>>(node)->GetPrimitives().size() == 0)
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
            SerializePrimitivesToXML<bool>(node, stringStream, depth);

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_byte:
        {
            SerializePrimitivesToXML<unsigned char>(node, stringStream, depth);

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_char:
        {
            SerializePrimitivesToXML<char>(node, stringStream, depth);

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_uint16:
        {
            SerializePrimitivesToXML<unsigned short>(node, stringStream, depth);

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_int16:
        {
            SerializePrimitivesToXML<short>(node, stringStream, depth);

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_uint32:
        {
            SerializePrimitivesToXML<unsigned int>(node, stringStream, depth);

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_int32:
        {
            SerializePrimitivesToXML<int>(node, stringStream, depth);

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_float:
        {
            SerializePrimitivesToXML<float>(node, stringStream, depth);

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_string:
        {
            SerializePrimitivesToXML<std::string>(node, stringStream, depth);

            break;
        }
        case CloakWorks::Reflection::FieldType::kFieldType_Object:
        {
            std::shared_ptr<ObjectNode> objectNode = std::static_pointer_cast<ObjectNode>(node);
            const std::vector<std::shared_ptr<Node>> children = objectNode->GetChildren();

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
            std::shared_ptr<ArrayNode> arrayNode = std::static_pointer_cast<ArrayNode>(node);
            const std::vector<std::shared_ptr<Node>> children = arrayNode->GetChildren();
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
                        SerializeArrayPrimitivesToXML<bool>(node, stringStream, depth);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_byte:
                    {
                        SerializeArrayPrimitivesToXML<unsigned char>(node, stringStream, depth);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_char:
                    {
                        SerializeArrayPrimitivesToXML<char>(node, stringStream, depth);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_uint16:
                    {
                        SerializeArrayPrimitivesToXML<unsigned short>(node, stringStream, depth);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_int16:
                    {
                        SerializeArrayPrimitivesToXML<short>(node, stringStream, depth);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_uint32:
                    {
                        SerializeArrayPrimitivesToXML<unsigned int>(node, stringStream, depth);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_int32:
                    {
                        SerializeArrayPrimitivesToXML<int>(node, stringStream, depth);

                        break;
                    }
                    case CloakWorks::Reflection::FieldType::kFieldType_float:
                    {
                        SerializeArrayPrimitivesToXML<float>(node, stringStream, depth);

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
