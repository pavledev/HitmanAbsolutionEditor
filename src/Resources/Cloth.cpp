#include "GLTFSDK/BufferBuilder.h"
#include "GLTFSDK/IStreamWriter.h"
#include "GLTFSDK/GLBResourceWriter.h"
#include "GLTFSDK/Serialize.h"

#include "Resources/TemplateEntity.h"
#include "Resources/RenderMaterialInstance.h"
#include "Resources/Texture.h"
#include "Registry/ResourceInfoRegistry.h"
#include "Registry/PropertyRegistry.h"
#include "Registry/ResourceIDRegistry.h"
#include "Hash.h"
#include "Utility/ResourceUtility.h"
#include "Resources/Cloth.h"
#include "Resources/FabricResourceEntity.h"
#include "IO/GLTFStreamWriter.h"

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

const std::shared_ptr<Cloth::Node> Cloth::ObjectNode::GetChildByName(const std::string& name)
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        if (children[i]->GetName() == name)
        {
            return children[i];
        }
    }

    return nullptr;
}

const std::shared_ptr<Cloth::Node> Cloth::ObjectNode::GetChildByClassName(const std::string& className)
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        if (children[i]->GetClassName() == className)
        {
            return children[i];
        }
    }

    return nullptr;
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
    BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

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

void Cloth::Export(const std::string& outputPath, const std::string& exportOption)
{
    if (exportOption.starts_with("Raw"))
    {
        ExportRawData(outputPath);
    }
    else if (exportOption.starts_with("CloakWorks XML"))
    {
        SerializeToXML(outputPath);
    }
    else if (exportOption.contains("OBJ"))
    {
        SerializeToXML(outputPath);
        ExportMesh(outputPath, true);
    }
    else
    {
        SerializeToXML(outputPath);
        ExportMesh(outputPath, false);
    }
}

void Cloth::GenerateVerticesAndIndices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    const std::shared_ptr<ObjectNode> shroudObject = std::static_pointer_cast<ObjectNode>(this->shroudObject);
    const std::shared_ptr<ObjectNode> simulationObject = std::static_pointer_cast<ObjectNode>(shroudObject->GetChildByName("SimObject"));
    const std::shared_ptr<ObjectNode> shapeDefinitionObject = std::static_pointer_cast<ObjectNode>(simulationObject->GetChildByName("shapeDefinition"));

    CloakWorks::ShapeDefinition shapeDefinition;
    const unsigned int rowCount = std::static_pointer_cast<PrimitiveNode<unsigned int>>(shapeDefinitionObject->GetChildByName("numRows"))->GetPrimitives()[0];
    const unsigned int columnCount = std::static_pointer_cast<PrimitiveNode<unsigned int>>(shapeDefinitionObject->GetChildByName("numColumns"))->GetPrimitives()[0];
    const unsigned int nodeCount = std::static_pointer_cast<PrimitiveNode<unsigned int>>(shapeDefinitionObject->GetChildByName("numNodes"))->GetPrimitives()[0];
    const std::shared_ptr<ArrayNode> startingPositionsNode = std::static_pointer_cast<ArrayNode>(shapeDefinitionObject->GetChildByName("startingPositions"));
    const std::shared_ptr<ArrayNode> startingFlagsNode = std::static_pointer_cast<ArrayNode>(shapeDefinitionObject->GetChildByName("startingFlags"));
    const std::vector<std::shared_ptr<Node>> startingPositionsNodeChildren = startingPositionsNode->GetChildren();
    const std::vector<std::shared_ptr<Node>> startingFlagsNodeChildren = startingFlagsNode->GetChildren();

    const size_t startingPositionsNodeChildrenCount = startingPositionsNodeChildren.size();
    const size_t startingFlagsNodeChildrenCount = startingFlagsNodeChildren.size();
    const size_t startingPositionCount = startingPositionsNodeChildrenCount / 3;
    std::vector<float> startingPositions;
    std::vector<Vector3> startingPositions2;
    size_t index = 0;

    startingPositions.resize(startingPositionsNodeChildrenCount);
    startingPositions2.resize(startingPositionCount);

    for (size_t i = 0; i < startingPositionsNodeChildrenCount; ++i)
    {
        startingPositions[i] = std::static_pointer_cast<PrimitiveNode<float>>(startingPositionsNodeChildren[i])->GetPrimitives()[0];
    }

    for (size_t i = 0; i < startingPositionsNodeChildrenCount / 12; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingPositions2[i * 4 + j].x = startingPositions[index++];
        }
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingPositions2[i * 4 + j].y = startingPositions[index++];
        }
        for (unsigned int j = 0; j < 4; ++j)
        {
            startingPositions2[i * 4 + j].z = startingPositions[index++];
        }
    }

    std::vector<int> startingFlags;

    startingFlags.resize(startingFlagsNodeChildrenCount);

    for (int i = 0; i < startingFlagsNodeChildrenCount; ++i)
    {
        startingFlags[i] = std::static_pointer_cast<PrimitiveNode<int>>(startingFlagsNodeChildren[i])->GetPrimitives()[0];
    }

    shapeDefinition.SetNumRows(rowCount);
    shapeDefinition.SetNumColumns(columnCount);
    shapeDefinition.SetNumNodes(nodeCount);
    shapeDefinition.SetStartingPositions(startingPositions);
    shapeDefinition.SetStartingFlags(startingFlags);

    const std::shared_ptr<ObjectNode> thickMeshControlObject = std::static_pointer_cast<ObjectNode>(simulationObject->GetChildByClassName("ThickMeshControl"));

    if (thickMeshControlObject)
    {
        const std::shared_ptr<ArrayNode> indicesNode = std::static_pointer_cast<ArrayNode>(thickMeshControlObject->GetChildByName("index32s"));
        const std::vector<std::shared_ptr<Node>> indicesNodeChildren = indicesNode->GetChildren();

        indices.resize(indicesNodeChildren.size());

        for (int i = 0; i < indicesNodeChildren.size(); ++i)
        {
            indices[i] = std::static_pointer_cast<PrimitiveNode<unsigned int>>(indicesNodeChildren[i])->GetPrimitives()[0];
        }

        std::vector<Vector3> startingNormals;

        if (shapeDefinitionObject->GetClassName() == "SheetShapeDefinition")
        {
            startingNormals = CloakWorks::SheetShapeDefinition::GenerateStartingNormals(shapeDefinition);
        }
        else if (shapeDefinitionObject->GetClassName() == "TubeShapeDefinition")
        {
            startingNormals = CloakWorks::TubeShapeDefinition::GenerateStartingNormals(shapeDefinition);
        }
        else if (shapeDefinitionObject->GetClassName() == "StrandShapeDefinition")
        {
            startingNormals = CloakWorks::StrandShapeDefinition::GenerateStartingNormals(shapeDefinition);
        }

        vertices = GenerateVerticesForThickMesh(shapeDefinition, startingPositions2, startingNormals, thickMeshControlObject);
    }
    else
    {
        if (shapeDefinitionObject->GetClassName() == "SheetShapeDefinition")
        {
            GenerateVerticesAndIndicesForSheetMesh(shapeDefinition, startingPositions2, vertices, indices);
        }
        else if (shapeDefinitionObject->GetClassName() == "TubeShapeDefinition")
        {
            GenerateVerticesAndIndicesForTubeMesh(shapeDefinition, startingPositions2, vertices, indices);
        }
        else if (shapeDefinitionObject->GetClassName() == "StrandShapeDefinition")
        {
            GenerateVerticesAndIndicesForStrandMesh(shapeDefinition, startingPositions2, vertices, indices);
        }
    }
}

std::shared_ptr<RenderMaterialInstance> Cloth::FindMaterialReference()
{
    std::shared_ptr<RenderMaterialInstance> matiResource;
    const std::vector<std::shared_ptr<Resource>>& backReferences = GetBackReferences();
    std::shared_ptr<TemplateEntity> tempResource;
    bool hasClotBackReference = false;
    ZRuntimeResourceID clotRuntimeResourceID = -1;

    for (size_t i = 0; i < backReferences.size(); ++i)
    {
        const ResourceInfoRegistry::ResourceInfo& backReferenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(backReferences[i]->GetHash());

        if (backReferenceInfo.type == "TEMP")
        {
            tempResource = std::static_pointer_cast<TemplateEntity>(backReferences[i]);

            break;
        }
        else if (backReferenceInfo.type == "CLOT")
        {
            std::shared_ptr<FabricResourceEntity> clotResource = std::static_pointer_cast<FabricResourceEntity>(backReferences[i]);
            const ResourceInfoRegistry::ResourceInfo& clotResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(clotResource->GetHash());

            clotResource->LoadResource(0, clotResourceInfo.headerLibraries[0].chunkIndex, clotResourceInfo.headerLibraries[0].indexInLibrary, false, true, false);

            const std::vector<std::shared_ptr<Resource>>& clotBackReferences = clotResource->GetBackReferences();

            for (size_t j = 0; j < clotBackReferences.size(); ++j)
            {
                const ResourceInfoRegistry::ResourceInfo& backReferenceInfo2 = ResourceInfoRegistry::GetInstance().GetResourceInfo(clotBackReferences[j]->GetHash());

                if (backReferenceInfo2.type == "TEMP")
                {
                    tempResource = std::static_pointer_cast<TemplateEntity>(clotBackReferences[j]);

                    break;
                }
            }

            hasClotBackReference = true;
            clotRuntimeResourceID = clotResource->GetRuntimeResourceID();

            break;
        }
    }

    const ResourceInfoRegistry::ResourceInfo& tempResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(tempResource->GetHash());

    tempResource->LoadResource(0, tempResourceInfo.headerLibraries[0].chunkIndex, tempResourceInfo.headerLibraries[0].indexInLibrary, true, false, true);
    tempResource->Deserialize();
    tempResource->DeleteResourceData();

    std::shared_ptr<STemplateEntity> templateEntity = tempResource->GetTemplateEntity();
    const std::vector<std::shared_ptr<Resource>>& tempReferences = tempResource->GetReferences();
    ZRuntimeResourceID materialID;

    for (size_t i = 0; i < templateEntity->entityTemplates.Size(); ++i)
    {
        const unsigned int referenceIndex = templateEntity->entityTemplates[i].entityTypeResourceIndex;
        const std::shared_ptr<Resource> reference = tempReferences[referenceIndex];

        if (hasClotBackReference && reference->GetRuntimeResourceID() != clotRuntimeResourceID)
        {
            continue;
        }

        TArray<SEntityTemplateProperty>& properties = templateEntity->entityTemplates[i].propertyValues;
        TArray<SEntityTemplateProperty>& postInitProperties = templateEntity->entityTemplates[i].postInitPropertyValues;
        ZRuntimeResourceID fabricResourceID;

        for (size_t j = 0; j < properties.Size(); ++j)
        {
            const std::string& propertyName = PropertyRegistry::GetInstance().GetPropertyName(properties[j].nPropertyID);

            if (!hasClotBackReference && propertyName == "m_FabricResourceID")
            {
                fabricResourceID = properties[j].value.Get<ZRuntimeResourceID>();
            }
            else if (propertyName == "m_MaterialID")
            {
                materialID = properties[j].value.Get<ZRuntimeResourceID>();
            }
        }

        if (hasClotBackReference && materialID.GetID() != -1)
        {
            break;
        }
        else if (fabricResourceID == GetRuntimeResourceID())
        {
            break;
        }

        for (size_t j = 0; j < postInitProperties.Size(); ++j)
        {
            const std::string& propertyName = PropertyRegistry::GetInstance().GetPropertyName(postInitProperties[j].nPropertyID);

            if (!hasClotBackReference && propertyName == "m_FabricResourceID")
            {
                fabricResourceID = postInitProperties[j].value.Get<ZRuntimeResourceID>();
            }
            else if (propertyName == "m_MaterialID")
            {
                materialID = postInitProperties[j].value.Get<ZRuntimeResourceID>();
            }
        }

        if (hasClotBackReference && materialID.GetID() != -1)
        {
            break;
        }
        else if (fabricResourceID == GetRuntimeResourceID())
        {
            break;
        }
    }

    if (materialID.GetID() != -1)
    {
        const std::string matiResourceID = ResourceIDRegistry::GetInstance().GetResourceID(materialID.GetID());
        const unsigned long long matiHash = Hash::GetMD5(matiResourceID);
        const ResourceInfoRegistry::ResourceInfo& matiResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(matiHash);
        matiResource = std::static_pointer_cast<RenderMaterialInstance>(ResourceUtility::CreateResource("MATI"));

        matiResource->SetHeaderLibraries(&matiResourceInfo.headerLibraries);
        matiResource->SetResourceID(matiResourceID);
        matiResource->LoadResource(0, matiResourceInfo.headerLibraries[0].chunkIndex, matiResourceInfo.headerLibraries[0].indexInLibrary, true, false, true);
        matiResource->Deserialize();
        matiResource->DeleteResourceData();
    }

    return matiResource;
}

void Cloth::ExportMesh(const std::string& outputPath, const bool exportToOBJ)
{
    std::vector<Cloth::Vertex> vertices;
    std::vector<unsigned int> indices;

    GenerateVerticesAndIndices(vertices, indices);

    const std::shared_ptr<ObjectNode> shroudObject = std::static_pointer_cast<ObjectNode>(this->shroudObject);
    const std::shared_ptr<ObjectNode> simulationObject = std::static_pointer_cast<ObjectNode>(shroudObject->GetChildByName("SimObject"));
    const std::string simulationObjectName = std::static_pointer_cast<PrimitiveNode<std::string>>(simulationObject->GetChildByName("name"))->GetPrimitives()[0];

    if (exportToOBJ)
    {
        ExportMeshToOBJ(vertices, indices, simulationObjectName, outputPath);
    }
    else
    {
        ExportMeshToGLB(vertices, indices, simulationObjectName, outputPath, true);
    }
}

void Cloth::ExportMeshToOBJ(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& simulationObjectName, const std::string& outputPath)
{
    const std::string objFilePath = std::format("{}\\{} Mesh.obj", outputPath, simulationObjectName);
    std::ofstream objFile = std::ofstream(objFilePath);

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        objFile << std::format("v {} {} {}\n", vertices[i].position.x, vertices[i].position.y, vertices[i].position.z);
    }

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        objFile << std::format("vn {} {} {}\n", vertices[i].normal.x, vertices[i].normal.y, vertices[i].normal.z);
    }

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        objFile << std::format("vt {} {}\n", vertices[i].textureCoordinates.x, -1 * vertices[i].textureCoordinates.y + 1);
    }

    std::shared_ptr<RenderMaterialInstance> matiResource = FindMaterialReference();

    if (matiResource)
    {
        const std::string materialResourceName = ResourceUtility::GetResourceName(matiResource->GetResourceID());
        const std::vector<std::shared_ptr<Resource>>& matiReferences = matiResource->GetReferences();
        std::vector<RenderMaterialInstance::Texture> textures;

        matiResource->GetTextures(matiResource, textures);

        for (size_t j = 0; j < textures.size(); ++j)
        {
            const unsigned int textureReferenceIndex = textures[j].textureReferenceIndex;
            const std::string textureResourceID = matiReferences[textureReferenceIndex]->GetResourceID();
            const std::string textureResourceName = ResourceUtility::GetResourceName(textureResourceID);
            const std::shared_ptr<Texture> textReference = std::static_pointer_cast<Texture>(matiReferences[textures[j].textureReferenceIndex]);
            const ResourceInfoRegistry::ResourceInfo& textReferenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(textReference->GetHash());

            textures[j].name = std::format("{}\\{}.tga", outputPath, textureResourceName);

            textReference->SetHeaderLibraries(&textReferenceInfo.headerLibraries);
            textReference->LoadResource(0, textReferenceInfo.headerLibraries[0].chunkIndex, textReferenceInfo.headerLibraries[0].indexInLibrary, false, false, true);
            textReference->Deserialize();
            textReference->ConvertTextureToTGA(textures[j].name);
            textReference->DeleteResourceData();
        }

        const std::string mtlFileName = std::format("{}.mtl", materialResourceName);
        const std::string mtlFilePath = std::format("{}\\{}", outputPath, mtlFileName);
        std::ofstream mtlFile = std::ofstream(mtlFilePath);

        mtlFile << "newmtl " << materialResourceName << std::endl;
        mtlFile << "Ka 1.000 1.000 1.000\nKd 1.000 1.000 1.000\nKs 0.000 0.000 0.000\nNs 1.0\nd 1.0\nillum 2\n";

        for (size_t j = 0; j < textures.size(); ++j)
        {
            if (textures[j].type == RenderMaterialInstance::Texture::Type::Diffuse)
            {
                mtlFile << "map_Kd " << textures[j].name << std::endl;
            }
            else if (textures[j].type == RenderMaterialInstance::Texture::Type::Normal)
            {
                mtlFile << "map_Bump " << textures[j].name << std::endl;
            }
            else if (textures[j].type == RenderMaterialInstance::Texture::Type::Specular)
            {
                mtlFile << "map_Ks " << textures[j].name << std::endl;
            }
        }

        mtlFile.close();

        objFile << "mtllib " << mtlFileName << std::endl;
        objFile << "usemtl " << materialResourceName << std::endl;
        objFile << "s 1" << std::endl;
    }

    const size_t indexCount = indices.size();

    for (int i = 0; i < indexCount / 3; ++i)
    {
        size_t vertexIndex = indices[i * 3] + 1;
        size_t vertexIndex2 = indices[i * 3 + 1] + 1;
        size_t vertexIndex3 = indices[i * 3 + 2] + 1;

        objFile << std::format("f {}/{}/{} {}/{}/{} {}/{}/{}\n", vertexIndex, vertexIndex, vertexIndex, vertexIndex2, vertexIndex2, vertexIndex2, vertexIndex3, vertexIndex3, vertexIndex3);
    }

    objFile.close();
}

void Cloth::ExportMeshToGLB(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& simulationObjectName, const std::string& outputPath, const bool rotate)
{
    std::vector<float> verticesMin;
    std::vector<float> verticesMax;

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        if (i == 0)
        {
            verticesMin.push_back(vertices[i].position.x);
            verticesMin.push_back(vertices[i].position.y);
            verticesMin.push_back(vertices[i].position.z);

            verticesMax.push_back(vertices[i].position.x);
            verticesMax.push_back(vertices[i].position.y);
            verticesMax.push_back(vertices[i].position.x);
        }
        else
        {
            if (vertices[i].position.x < verticesMin[0])
            {
                verticesMin[0] = vertices[i].position.x;
            }

            if (vertices[i].position.x > verticesMax[0])
            {
                verticesMax[0] = vertices[i].position.x;
            }

            if (vertices[i].position.y < verticesMin[1])
            {
                verticesMin[1] = vertices[i].position.y;
            }

            if (vertices[i].position.y > verticesMax[1])
            {
                verticesMax[1] = vertices[i].position.y;
            }

            if (vertices[i].position.z < verticesMin[2])
            {
                verticesMin[2] = vertices[i].position.z;
            }

            if (vertices[i].position.z > verticesMax[2])
            {
                verticesMax[2] = vertices[i].position.z;
            }
        }
    }

    const std::string glbFilePath = std::format("{}\\{} Mesh2.glb", outputPath, simulationObjectName);
    const std::filesystem::path glbFilePath2 = glbFilePath;
    const std::filesystem::path glbFileName = glbFilePath2.filename();

    std::unique_ptr<GLTFStreamWriter> streamWriter = std::make_unique<GLTFStreamWriter>(glbFilePath2.parent_path().string());
    std::unique_ptr<Microsoft::glTF::ResourceWriter> resourceWriter = std::make_unique<Microsoft::glTF::GLBResourceWriter>(std::move(streamWriter));

    if (!resourceWriter)
    {
        throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
    }

    Microsoft::glTF::Document document;
    Microsoft::glTF::BufferBuilder bufferBuilder(std::move(resourceWriter));

    bufferBuilder.AddBuffer(Microsoft::glTF::GLB_BUFFER_ID);

    std::string materialID;
    const std::vector<std::shared_ptr<Resource>>& primReferences = GetReferences();
    std::shared_ptr<RenderMaterialInstance> matiResource = FindMaterialReference();
    std::vector<RenderMaterialInstance::Texture> textures;

    matiResource->GetTextures(matiResource, textures);

    const std::string materialResourceName = ResourceUtility::GetResourceName(matiResource->GetResourceID());
    const std::vector<std::shared_ptr<Resource>>& matiReferences = matiResource->GetReferences();
    Microsoft::glTF::Material material;

    material.name = materialResourceName;

    for (size_t i = 0; i < textures.size(); ++i)
    {
        const unsigned int textureReferenceIndex = textures[i].textureReferenceIndex;
        const std::string textureResourceID = matiReferences[textureReferenceIndex]->GetResourceID();
        const std::string textureResourceName = ResourceUtility::GetResourceName(textureResourceID);
        const std::shared_ptr<Texture> textReference = std::static_pointer_cast<Texture>(matiReferences[textures[i].textureReferenceIndex]);
        const ResourceInfoRegistry::ResourceInfo& textReferenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(textReference->GetHash());
        DirectX::Blob blob;

        textReference->SetHeaderLibraries(&textReferenceInfo.headerLibraries);
        textReference->LoadResource(0, textReferenceInfo.headerLibraries[0].chunkIndex, textReferenceInfo.headerLibraries[0].indexInLibrary, false, false, true);
        textReference->Deserialize();
        textReference->ConvertTextureToPNG(&blob);

        if (textures[i].type == RenderMaterialInstance::Texture::Type::Normal ||
            textures[i].type == RenderMaterialInstance::Texture::Type::Diffuse ||
            textures[i].type == RenderMaterialInstance::Texture::Type::Emissive)
        {
            Microsoft::glTF::Image image;
            Microsoft::glTF::Texture gltfTexture;

            image.bufferViewId = bufferBuilder.AddBufferView(blob.GetBufferPointer(), blob.GetBufferSize()).id;
            image.mimeType = "image/png";

            std::string imageID = document.images.Append(std::move(image), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

            gltfTexture.imageId = imageID;

            std::string textureID = document.textures.Append(std::move(gltfTexture), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

            if (textures[i].type == RenderMaterialInstance::Texture::Type::Normal)
            {
                material.normalTexture.textureId = textureID;
            }
            else if (textures[i].type == RenderMaterialInstance::Texture::Type::Diffuse)
            {
                material.metallicRoughness.baseColorTexture.textureId = textureID;
            }
            else if (textures[i].type == RenderMaterialInstance::Texture::Type::Emissive)
            {
                material.emissiveTexture.textureId = textureID;
            }
        }

        textReference->DeleteResourceData();
    }

    if (textures.size() > 0)
    {
        material.metallicRoughness.metallicFactor = 0.0f;
        material.metallicRoughness.roughnessFactor = 0.0f;

        materialID = document.materials.Append(std::move(material), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;
    }

    std::vector<std::string> nodeIDs;
    Microsoft::glTF::Node node;

    node.name = name;

    if (rotate)
    {
        node.rotation = Microsoft::glTF::Quaternion::Quaternion(static_cast<float>(Math::Sqrt(2.0) / 2.0), 0.f, 0.f, static_cast<float>(-Math::Sqrt(2.0) / 2.0));
    }

    node.children.push_back(std::to_string(1));

    document.nodes.Append(std::move(node), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty);

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> textureCoordinates;

    for (int i = 0; i < vertices.size(); ++i)
    {
        positions.push_back(vertices[i].position.x);
        positions.push_back(vertices[i].position.y);
        positions.push_back(vertices[i].position.z);
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        normals.push_back(vertices[i].normal.x);
        normals.push_back(vertices[i].normal.y);
        normals.push_back(vertices[i].normal.z);
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        tangents.push_back(vertices[i].tangent.x);
        tangents.push_back(vertices[i].tangent.y);
        tangents.push_back(vertices[i].tangent.z);
        tangents.push_back(vertices[i].tangent.w);
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        textureCoordinates.push_back(vertices[i].textureCoordinates.x);
        textureCoordinates.push_back(vertices[i].textureCoordinates.y);
    }

    Microsoft::glTF::MeshPrimitive meshPrimitive;

    bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ELEMENT_ARRAY_BUFFER);
    meshPrimitive.indicesAccessorId = bufferBuilder.AddAccessor(indices,
        { Microsoft::glTF::AccessorType::TYPE_SCALAR, Microsoft::glTF::COMPONENT_UNSIGNED_INT }).id;

    bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);
    meshPrimitive.attributes["POSITION"] = bufferBuilder.AddAccessor(positions,
        { Microsoft::glTF::TYPE_VEC3, Microsoft::glTF::COMPONENT_FLOAT, false,
         std::move(verticesMin),
         std::move(verticesMax) }).id;

    bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);
    meshPrimitive.attributes["NORMAL"] = bufferBuilder.AddAccessor(normals,
        { Microsoft::glTF::TYPE_VEC3, Microsoft::glTF::COMPONENT_FLOAT }).id;

    bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);
    meshPrimitive.attributes["TEXCOORD_0"] = bufferBuilder.AddAccessor(textureCoordinates,
        { Microsoft::glTF::TYPE_VEC2, Microsoft::glTF::COMPONENT_FLOAT }).id;

    bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);
    meshPrimitive.attributes["TANGENT"] = bufferBuilder.AddAccessor(tangents,
        { Microsoft::glTF::TYPE_VEC4, Microsoft::glTF::COMPONENT_FLOAT }).id;

    meshPrimitive.materialId = materialID;

    Microsoft::glTF::Mesh mesh;
    mesh.primitives.push_back(std::move(meshPrimitive));
    mesh.name = std::format("{}_{}", name, 0);

    auto meshId = document.meshes.Append(std::move(mesh), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

    Microsoft::glTF::Node node2;
    node2.meshId = meshId;

    nodeIDs.push_back(document.nodes.Append(std::move(node2), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id);

    Microsoft::glTF::Scene scene;

    for (const auto& nodeID : nodeIDs)
    {
        scene.nodes.push_back(nodeID);
    }

    document.SetDefaultScene(std::move(scene), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty);

    bufferBuilder.Output(document);

    std::string manifest;

    try
    {
        manifest = Serialize(document, Microsoft::glTF::SerializeFlags::None);
    }
    catch (const Microsoft::glTF::GLTFException& ex)
    {
        std::stringstream ss;

        ss << "Microsoft::glTF::Serialize failed: ";
        ss << ex.what();

        throw std::runtime_error(ss.str());
    }

    auto& gltfResourceWriter = bufferBuilder.GetResourceWriter();

    auto glbResourceWriter = dynamic_cast<Microsoft::glTF::GLBResourceWriter*>(&gltfResourceWriter);

    glbResourceWriter->Flush(manifest, glbFileName.string());
}

std::vector<Cloth::Vertex> Cloth::GenerateVerticesForThickMesh(CloakWorks::ShapeDefinition& shapeDefinition, const std::vector<Vector3>& startingPositions, const std::vector<Vector3>& startingNormals, const std::shared_ptr<ObjectNode> thickMeshControlObject)
{
    const unsigned int vertexCount = std::static_pointer_cast<PrimitiveNode<unsigned int>>(thickMeshControlObject->GetChildByName("mappedVertCount"))->GetPrimitives()[0];
    const std::shared_ptr<ArrayNode> bindingOffsetsNode = std::static_pointer_cast<ArrayNode>(thickMeshControlObject->GetChildByName("bindingOffsets"));
    const std::shared_ptr<ArrayNode> triangleIndicesNode = std::static_pointer_cast<ArrayNode>(thickMeshControlObject->GetChildByName("triIndices"));
    const std::shared_ptr<ArrayNode> textureCoordinatesNode = std::static_pointer_cast<ArrayNode>(thickMeshControlObject->GetChildByName("texCoords"));
    const std::vector<std::shared_ptr<Node>> bindingOffsetsNodeChildren = bindingOffsetsNode->GetChildren();
    const std::vector<std::shared_ptr<Node>> triangleIndicesNodeChildren = triangleIndicesNode->GetChildren();
    const std::vector<std::shared_ptr<Node>> textureCoordinatesNodeChildren = textureCoordinatesNode->GetChildren();
    std::vector<Vertex> vertices;

    vertices.resize(vertexCount);

    unsigned int bindingOffsetIndex = 0;

    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        MappingInfo mappingInfo;

        mappingInfo.barycentricCoordinates.x = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.barycentricCoordinates.y = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.barycentricCoordinates.z = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.barycentricCoordinates.w = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];

        mappingInfo.positionOffset.x = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.positionOffset.y = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.positionOffset.z = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.positionOffset.w = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];

        mappingInfo.normalOffset.x = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.normalOffset.y = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.normalOffset.z = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.normalOffset.w = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];

        mappingInfo.tangentOffset.x = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.tangentOffset.y = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.tangentOffset.z = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];
        mappingInfo.tangentOffset.w = std::static_pointer_cast<PrimitiveNode<float>>(bindingOffsetsNodeChildren[bindingOffsetIndex++])->GetPrimitives()[0];

        const unsigned int triangleIndex1 = std::static_pointer_cast<PrimitiveNode<unsigned int>>(triangleIndicesNodeChildren[i * 3])->GetPrimitives()[0];
        const unsigned int triangleIndex2 = std::static_pointer_cast<PrimitiveNode<unsigned int>>(triangleIndicesNodeChildren[i * 3 + 1])->GetPrimitives()[0];
        const unsigned int triangleIndex3 = std::static_pointer_cast<PrimitiveNode<unsigned int>>(triangleIndicesNodeChildren[i * 3 + 2])->GetPrimitives()[0];

        const Vector3 a = startingPositions[triangleIndex1];
        const Vector3 b = startingPositions[triangleIndex2];
        const Vector3 c = startingPositions[triangleIndex3];
        const Vector3 startingNormal1 = startingNormals[triangleIndex1];
        const Vector3 startingNormal2 = startingNormals[triangleIndex2];
        const Vector3 startingNormal3 = startingNormals[triangleIndex3];

        const Vector3 directionAB = (b - a).Normalized();
        const Vector3 directionAC = (c - a).Normalized();

        const Vector3 bindingPosition = Vector3(a * mappingInfo.barycentricCoordinates.x + b * mappingInfo.barycentricCoordinates.y + c * mappingInfo.barycentricCoordinates.z);
        const Vector3 bindingNormal = Vector3(startingNormal1 * mappingInfo.barycentricCoordinates.x + startingNormal2 * mappingInfo.barycentricCoordinates.y + startingNormal3 * mappingInfo.barycentricCoordinates.z);

        const Vector3 displacementNormalProjA = (bindingPosition - a) * startingNormal1;
        const Vector3 displacementNormalProjB = (bindingPosition - b) * startingNormal2;
        const Vector3 displacementNormalProjC = (bindingPosition - c) * startingNormal3;
        const float weightedDisplacementA = (displacementNormalProjA.x + displacementNormalProjA.y + displacementNormalProjA.z) * mappingInfo.barycentricCoordinates.x * mappingInfo.barycentricCoordinates.x;
        const float weightedDisplacementB = (displacementNormalProjB.x + displacementNormalProjB.y + displacementNormalProjB.z) * mappingInfo.barycentricCoordinates.y * mappingInfo.barycentricCoordinates.y;
        const float weightedDisplacementC = (displacementNormalProjC.x + displacementNormalProjC.y + displacementNormalProjC.z) * mappingInfo.barycentricCoordinates.z * mappingInfo.barycentricCoordinates.z;
        const float totalDisplacementInfluence = (weightedDisplacementA + weightedDisplacementB + weightedDisplacementC) * -1.f;
        const Vector3 adjustedBindingPosition = bindingNormal * totalDisplacementInfluence + bindingPosition;

        const Vector3 vertexPosition = Vector3(directionAB * mappingInfo.positionOffset.x + bindingNormal * mappingInfo.positionOffset.y + directionAC * mappingInfo.positionOffset.z) + adjustedBindingPosition;
        const Vector3 vertexNormal = Vector3(directionAB * mappingInfo.normalOffset.x + bindingNormal * mappingInfo.normalOffset.y + directionAC * mappingInfo.normalOffset.z);
        const Vector3 vertexTangent = Vector3(directionAB * mappingInfo.tangentOffset.x + bindingNormal * mappingInfo.tangentOffset.y + directionAC * mappingInfo.tangentOffset.z);

        const float textureCoordinateX = std::static_pointer_cast<PrimitiveNode<float>>(textureCoordinatesNodeChildren[i * 2])->GetPrimitives()[0];
        const float textureCoordinateY = std::static_pointer_cast<PrimitiveNode<float>>(textureCoordinatesNodeChildren[i * 2 + 1])->GetPrimitives()[0];

        vertices[i].position = vertexPosition;
        vertices[i].normal = vertexNormal;
        vertices[i].tangent = Vector4(vertexTangent.x, vertexTangent.y, vertexTangent.z, mappingInfo.tangentOffset.w);
        vertices[i].textureCoordinates = Vector2(textureCoordinateX, textureCoordinateY);
    }

    return vertices;
}

void Cloth::GenerateVerticesAndIndicesForSheetMesh(CloakWorks::ShapeDefinition& shapeDefinition, const std::vector<Vector3>& startingPositions, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    const CloakWorks::SheetMeshControlInstance sheetMeshControlInstance = CloakWorks::SheetMeshControlInstance(shapeDefinition);
    const unsigned int vertexCount = shapeDefinition.GetNumRows() * shapeDefinition.GetNumColumns();
    const std::vector<Vector3> startingNormals = CloakWorks::SheetShapeDefinition::GenerateStartingNormals(shapeDefinition);
    std::vector<Vector2> textureCoordinates;

    vertices.resize(vertexCount);

    sheetMeshControlInstance.FillIndexBuffer(indices);
    sheetMeshControlInstance.FillTexCoordsBuffer(textureCoordinates);

    const std::vector<Vector4> tangents = CalculateTangents(shapeDefinition, startingPositions, startingNormals, indices, textureCoordinates);

    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        vertices[i].position = startingPositions[i];
        vertices[i].normal = startingNormals[i];
        vertices[i].tangent = tangents[i];
        vertices[i].textureCoordinates = textureCoordinates[i];
    }
}

void Cloth::GenerateVerticesAndIndicesForTubeMesh(CloakWorks::ShapeDefinition& shapeDefinition, const std::vector<Vector3>& startingPositions, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    const CloakWorks::TubeMeshControlInstance tubeMeshControlInstance = CloakWorks::TubeMeshControlInstance(shapeDefinition);
    const unsigned int vertexCount = shapeDefinition.GetNumRows() * shapeDefinition.GetNumColumns();
    const std::vector<Vector3> startingNormals = CloakWorks::TubeShapeDefinition::GenerateStartingNormals(shapeDefinition);
    std::vector<Vector2> textureCoordinates;

    vertices.resize(vertexCount);

    tubeMeshControlInstance.FillIndexBuffer(indices);
    tubeMeshControlInstance.FillTexCoordsBuffer(textureCoordinates);

    const std::vector<Vector4> tangents = CalculateTangents(shapeDefinition, startingPositions, startingNormals, indices, textureCoordinates);

    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        vertices[i].position = startingPositions[i];
        vertices[i].normal = startingNormals[i];
        vertices[i].tangent = tangents[i];
        vertices[i].textureCoordinates = textureCoordinates[i];
    }
}

void Cloth::GenerateVerticesAndIndicesForStrandMesh(CloakWorks::ShapeDefinition& shapeDefinition, const std::vector<Vector3>& startingPositions, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    const CloakWorks::StrandMeshControlInstance strandMeshControlInstance = CloakWorks::StrandMeshControlInstance(shapeDefinition);
    const unsigned int vertexCount = shapeDefinition.GetNumRows() * shapeDefinition.GetNumColumns();
    const std::vector<Vector3> startingNormals = CloakWorks::StrandShapeDefinition::GenerateStartingNormals(shapeDefinition);
    std::vector<Vector2> textureCoordinates;

    vertices.resize(vertexCount);

    strandMeshControlInstance.FillIndexBuffer(indices);
    strandMeshControlInstance.FillTexCoordsBuffer(textureCoordinates);

    const std::vector<Vector4> tangents = CalculateTangents(shapeDefinition, startingPositions, startingNormals, indices, textureCoordinates);

    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        vertices[i].position = startingPositions[i];
        vertices[i].normal = startingNormals[i];
        vertices[i].tangent = tangents[i];
        vertices[i].textureCoordinates = textureCoordinates[i];
    }
}

std::vector<Vector4> Cloth::CalculateTangents(CloakWorks::ShapeDefinition& shapeDefinition, const std::vector<Vector3>& startingPositions, std::vector<Vector3> startingNormals, const std::vector<unsigned int>& triangleIndices, const std::vector<Vector2>& textureCoordinates)
{
    std::vector<Vector4> tangents1;

    tangents1.resize(startingPositions.size());

    for (size_t i = 0; i < triangleIndices.size(); i += 3)
    {
        const unsigned int triangleIndex1 = triangleIndices[i];
        const unsigned int triangleIndex2 = triangleIndices[i + 1];
        const unsigned int triangleIndex3 = triangleIndices[i + 2];
        const Vector3 a = startingPositions[triangleIndex1];
        const Vector3 b = startingPositions[triangleIndex2];
        const Vector3 c = startingPositions[triangleIndex3];
        const Vector2 textureCoordinates1 = textureCoordinates[triangleIndex1];
        const Vector2 textureCoordinates2 = textureCoordinates[triangleIndex2];
        const Vector2 textureCoordinates3 = textureCoordinates[triangleIndex3];

        const float x1 = b.x - a.x;
        const float x2 = c.x - a.x;
        const float y1 = b.y - a.y;
        const float y2 = c.y - a.y;
        const float z1 = b.z - a.z;
        const float z2 = c.z - a.z;
        const float s1 = textureCoordinates2.x - textureCoordinates1.x;
        const float s2 = textureCoordinates3.x - textureCoordinates1.x;
        const float t1 = textureCoordinates2.y - textureCoordinates1.y;
        const float t2 = textureCoordinates3.y - textureCoordinates1.y;

        const float r = 1.0f / (s1 * t2 - s2 * t1);
        const Vector3 uDirection = Vector3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);

        tangents1[triangleIndex1] += uDirection;
        tangents1[triangleIndex2] += uDirection;
        tangents1[triangleIndex3] += uDirection;
    }

    std::vector<Vector4> tangents2;

    tangents2.resize(startingPositions.size());

    for (size_t i = 0; i < startingPositions.size(); ++i)
    {
        const Vector3 startingNormal = startingNormals[i];
        Vector3 tangent = tangents1[i];

        //Gram-Schmidt orthogonalize
        tangent = (tangent - startingNormal * Vector3::Dot(startingNormal, tangent)).Normalized();

        Vector4 tangent2 = tangent;
        Vector3 bitangent = Vector3::Cross(startingNormal, tangent);

        //Calculate handedness
        tangent2.w = (Vector3::Dot(Vector3::Cross(startingNormal, tangent), bitangent) < 0.f) ? -1.f : 1.f;

        tangents2[i] = tangent2;
    }

    return tangents2;
}

std::vector<Vector3> Cloth::CalculateBitangents(std::vector<Vertex>& vertices)
{
    std::vector<Vector3> bitangents;

    bitangents.resize(vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        bitangents[i] = Vector3::Cross(vertices[i].normal, Vector3(vertices[i].tangent)) * vertices[i].tangent.w;
    }

    return bitangents;
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

void Cloth::SerializeToXML(const std::string& outputFilePath)
{
    std::stringstream stringStream;

    stringStream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;

    SerializeToXML(shroudObject, stringStream, 0);

    std::string xmlOutput = stringStream.str();
    std::ofstream ofstream = std::ofstream(outputFilePath);

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
