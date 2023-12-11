#include "GLTFSDK/BufferBuilder.h"
#include "GLTFSDK/IStreamWriter.h"
#include "GLTFSDK/GLBResourceWriter.h"
#include "GLTFSDK/Serialize.h"

#include "Glacier/RenderPrimitive/SPrimObject.h"

#include "Resources/RenderPrimitive.h"
#include "Logger.h"
#include "Utility/StringUtility.h"
#include "IO/GLTFStreamWriter.h"
#include "Utility/Math.h"
#include "IO/BinaryWriter.h"
#include "Registry/ResourceInfoRegistry.h"
#include "Utility/ResourceUtility.h"
#include "Resources/Texture.h"

RenderPrimitive::~RenderPrimitive()
{
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		delete meshes[i];
	}

	meshes.clear();
}

const SPrimObjectHeader& RenderPrimitive::GetPrimObjectHeader() const
{
	return primObjectHeader;
}

const unsigned int RenderPrimitive::GetBoneRigResourceIndex() const
{
	return primObjectHeader.lBoneRigResourceIndex;
}

const std::vector<RenderPrimitive::Mesh*>& RenderPrimitive::GetMeshes() const
{
	return meshes;
}

const BoneRig* RenderPrimitive::GetBoneRig() const
{
	return boneRig;
}

const Physics* RenderPrimitive::GetPhysics() const
{
	return physics;
}

BoneRig* RenderPrimitive::GetBoneRig()
{
	return boneRig;
}

void RenderPrimitive::SetBoneRig(BoneRig* boneRig)
{
	this->boneRig = boneRig;
}

void RenderPrimitive::SetPhysics(Physics* physics)
{
	this->physics = physics;
}

void RenderPrimitive::Deserialize(const std::string& filePath)
{
	BinaryReader binaryReader = BinaryReader(filePath);

	Deserialize(binaryReader);
}

void RenderPrimitive::Deserialize(void* buffer, const unsigned int dataSize)
{
	BinaryReader binaryReader = BinaryReader(buffer, dataSize);

	Deserialize(binaryReader);
}

void RenderPrimitive::Deserialize(BinaryReader& binaryReader)
{
	const unsigned int primaryOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(primaryOffset, SeekOrigin::Begin);

	primObjectHeader = binaryReader.Read<SPrimObjectHeader>();
	const SPrimHeader::EPrimType primType = static_cast<SPrimHeader::EPrimType>(primObjectHeader.lType);
	const SPrimObjectHeader::PROPERTY_FLAGS propertyFlags = static_cast<SPrimObjectHeader::PROPERTY_FLAGS>(primObjectHeader.lPropertyFlags);
	const bool hasHighResolutionPositions = (propertyFlags & SPrimObjectHeader::PROPERTY_FLAGS::HAS_HIRES_POSITIONS) == SPrimObjectHeader::PROPERTY_FLAGS::HAS_HIRES_POSITIONS;

	if (primType != SPrimHeader::EPrimType::PTOBJECTHEADER)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "{} prim header type isn't supported", SPrimHeader::ConvertPrimTypeToString(primType));

		return;
	}

	std::vector<unsigned int> objectOffsets;

	objectOffsets.reserve(primObjectHeader.lNumObjects);
	meshes.reserve(primObjectHeader.lNumObjects);

	binaryReader.Seek(primObjectHeader.lObjectTable, SeekOrigin::Begin);

	for (unsigned int i = 0; i < primObjectHeader.lNumObjects; ++i)
	{
		const unsigned int objectOffset = binaryReader.Read<unsigned int>();

		objectOffsets.push_back(objectOffset);
	}

	for (unsigned int i = 0; i < primObjectHeader.lNumObjects; ++i)
	{
		binaryReader.Seek(objectOffsets[i] + 2, SeekOrigin::Begin);

		const SPrimHeader::EPrimType primType = static_cast<SPrimHeader::EPrimType>(binaryReader.Read<unsigned short>());

		if (primType != SPrimHeader::EPrimType::PTMESH)
		{
			continue;
		}

		const SPrimObject::SUBTYPE subType = static_cast<SPrimObject::SUBTYPE>(binaryReader.Read<unsigned char>());

		binaryReader.Seek(objectOffsets[i], SeekOrigin::Begin);

		switch (subType)
		{
			case SPrimObject::SUBTYPE::SUBTYPE_STANDARD:
			{
				StandardMesh* standardMesh = new StandardMesh();

				standardMesh->Deserialize(binaryReader, hasHighResolutionPositions);

				meshes.push_back(standardMesh);

				break;
			}
			case SPrimObject::SUBTYPE::SUBTYPE_LINKED:
			{
				LinkedMesh* linkedMesh = new LinkedMesh();

				linkedMesh->Deserialize(binaryReader, hasHighResolutionPositions);

				meshes.push_back(linkedMesh);

				break;
			}
			case SPrimObject::SUBTYPE::SUBTYPE_WEIGHTED:
			{
				WeightedMesh* weightedMesh = new WeightedMesh();

				weightedMesh->Deserialize(binaryReader, hasHighResolutionPositions);

				meshes.push_back(weightedMesh);

				break;
			}
			default:
				Logger::GetInstance().Log(Logger::Level::Error, "{} prim object sub type isn't supported", SPrimObject::ConvertSubTypeToString(subType));
				break;
		}
	}
}

RenderPrimitive::Mesh::Mesh()
{
	vertexBuffer = nullptr;
	vertexBufferSize = 0;
}

RenderPrimitive::Mesh::~Mesh()
{
	delete[] vertexBuffer;
}

void RenderPrimitive::Mesh::ReadIndices(BinaryReader& binaryReader)
{
	indices.reserve(primSubMesh.lNumIndices);

	for (unsigned int i = 0; i < primSubMesh.lNumIndices; ++i)
	{
		indices.push_back(binaryReader.Read<unsigned short>());
	}
}

void RenderPrimitive::Mesh::ReadVertexPosition(BinaryReader& binaryReader, const unsigned int vertexIndex, const bool hasHighResolutionPositions, const Vector4& scale, const Vector4& bias)
{
	if (hasHighResolutionPositions)
	{
		float x = binaryReader.Read<float>();
		float y = binaryReader.Read<float>();
		float z = binaryReader.Read<float>();

		vertices[vertexIndex].position = Vector4(x, y, z, 1.0f);
	}
	else
	{
		short x = binaryReader.Read<short>();
		short y = binaryReader.Read<short>();
		short z = binaryReader.Read<short>();
		short w = binaryReader.Read<short>();

		vertices[vertexIndex].position = Vector4(
			static_cast<float>(x * scale.x) / std::numeric_limits<short>::max() + bias.x,
			static_cast<float>(y * scale.y) / std::numeric_limits<short>::max() + bias.y,
			static_cast<float>(z * scale.z) / std::numeric_limits<short>::max() + bias.z,
			static_cast<float>(w * scale.w) / std::numeric_limits<short>::max() + bias.w);
	}
}

void RenderPrimitive::Mesh::ReadVertexWeighsAndBoneRemapValues(BinaryReader& binaryReader, const unsigned int vertexIndex)
{
	unsigned char weightA = binaryReader.Read<unsigned char>();
	unsigned char weightB = binaryReader.Read<unsigned char>();
	unsigned char weightC = binaryReader.Read<unsigned char>();
	unsigned char weightD = binaryReader.Read<unsigned char>();

	if (weightA == 1)
	{
		weightA = 0;
		weightD += 1;
	}

	vertices[vertexIndex].weights[0] = static_cast<float>(weightA) / 255;
	vertices[vertexIndex].weights[1] = static_cast<float>(weightB) / 255;
	vertices[vertexIndex].weights[2] = static_cast<float>(weightC) / 255;
	vertices[vertexIndex].weights[3] = static_cast<float>(weightD) / 255;

	for (unsigned char i = 0; i < 4; ++i)
	{
		vertices[vertexIndex].boneRemapValues[i] = binaryReader.Read<unsigned char>();
	}
}

void RenderPrimitive::Mesh::ReadVertexNormal(BinaryReader& binaryReader, const unsigned int vertexIndex)
{
	float x = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;
	float y = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;
	float z = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;
	float w = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;

	vertices[vertexIndex].normal = Vector4(x, y, z, w);
}

void RenderPrimitive::Mesh::ReadVertexTangent(BinaryReader& binaryReader, const unsigned int vertexIndex)
{
	float x = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;
	float y = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;
	float z = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;
	float w = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;

	vertices[vertexIndex].tangent = Vector4(x, y, z, w);
}

void RenderPrimitive::Mesh::ReadVertexBinormal(BinaryReader& binaryReader, const unsigned int vertexIndex)
{
	float x = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;
	float y = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;
	float z = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;
	float w = ((2.0f * static_cast<float>(binaryReader.Read<unsigned char>())) / 255.0f) - 1.0f;

	vertices[vertexIndex].binormal = Vector4(x, y, z, w);
}

void RenderPrimitive::Mesh::ReadVertexUVs(BinaryReader& binaryReader, const unsigned int vertexIndex, const Vector2& scale, const Vector2& bias)
{
	float x = scale.x * static_cast<float>(binaryReader.Read<short>()) / std::numeric_limits<short>::max() + bias.x;
	float y = scale.y * static_cast<float>(binaryReader.Read<short>()) / std::numeric_limits<short>::max() + bias.y;

	vertices[vertexIndex].uv.push_back(Vector2(x, y));
}

void RenderPrimitive::Mesh::ReadVertexColor(BinaryReader& binaryReader, const unsigned int vertexIndex)
{
	unsigned char r = binaryReader.Read<unsigned char>();
	unsigned char g = binaryReader.Read<unsigned char>();
	unsigned char b = binaryReader.Read<unsigned char>();
	unsigned char a = binaryReader.Read<unsigned char>();

	vertices[vertexIndex].color = ColorRGBA(r, g, b, a);
}

const SPrimObject::SUBTYPE RenderPrimitive::Mesh::GetSubType() const
{
	return static_cast<SPrimObject::SUBTYPE>(primSubMesh.lSubType);
}

const unsigned int RenderPrimitive::Mesh::GetIndexCount() const
{
	return primSubMesh.lNumIndices;
}

const unsigned int RenderPrimitive::Mesh::GetVertexCount() const
{
	return primSubMesh.lNumVertices;
}

const std::vector<unsigned short>& RenderPrimitive::Mesh::GetIndices() const
{
	return indices;
}

const std::vector<RenderPrimitive::Vertex>& RenderPrimitive::Mesh::GetVertices() const
{
	return vertices;
}

std::vector<RenderPrimitive::Vertex>& RenderPrimitive::Mesh::GetVertices()
{
	return vertices;
}

const std::vector<float> RenderPrimitive::Mesh::GetPositions() const
{
	std::vector<float> positions;

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		positions.push_back(vertices[i].position.x);
		positions.push_back(vertices[i].position.y);
		positions.push_back(vertices[i].position.z);
	}

	return positions;
}

const std::vector<float> RenderPrimitive::Mesh::GetNormals() const
{
	std::vector<float> normals;

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		Vector3 normal = Vector3(vertices[i].normal);

		normal = normal.Normalized();

		normals.push_back(normal.x);
		normals.push_back(normal.y);
		normals.push_back(normal.z);
	}

	return normals;
}

const std::vector<std::vector<float>> RenderPrimitive::Mesh::GetUVs() const
{
	std::vector<std::vector<float>> uvs;

	for (size_t i = 0; i < vertices[i].uv.size(); ++i)
	{
		std::vector<float> uv;

		for (size_t j = 0; j < vertices.size(); ++j)
		{
			uv.push_back(vertices[j].uv[i].x);
			uv.push_back(vertices[j].uv[i].y);
		}

		uvs.push_back(uv);
	}

	return uvs;
}

const std::vector<float> RenderPrimitive::Mesh::GetWeights() const
{
	std::vector<float> weights;

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		for (unsigned char j = 0; j < 4; ++j)
		{
			weights.push_back(vertices[i].weights[j]);
		}
	}

	return weights;
}

const std::vector<unsigned char> RenderPrimitive::Mesh::GetBoneRemapValues() const
{
	std::vector<unsigned char> boneRemapValues;

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		for (unsigned char j = 0; j < 4; ++j)
		{
			boneRemapValues.push_back(vertices[i].boneRemapValues[j]);
		}
	}

	return boneRemapValues;
}

const std::vector<unsigned char> RenderPrimitive::Mesh::GetColors() const
{
	std::vector<unsigned char> colors;

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		colors.push_back(vertices[i].color.r);
		colors.push_back(vertices[i].color.g);
		colors.push_back(vertices[i].color.b);
		colors.push_back(vertices[i].color.a);
	}

	return colors;
}

const Vector3 RenderPrimitive::Mesh::GetBoundingBoxMin() const
{
	return primSubMesh.vBoundingBoxMin;
}

const Vector3 RenderPrimitive::Mesh::GetBoundingBoxMax() const
{
	return primSubMesh.vBoundingBoxMax;
}

const void* RenderPrimitive::Mesh::GetVertexBuffer() const
{
	return vertexBuffer;
}

const unsigned int RenderPrimitive::Mesh::GetVertexBufferSize() const
{
	return vertexBufferSize;
}

const unsigned int RenderPrimitive::Mesh::GetStride() const
{
	return sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector2);
}

void RenderPrimitive::Mesh::CreateVertexBuffer()
{
	SPrimObject::SUBTYPE subType = static_cast<SPrimObject::SUBTYPE>(primSubMesh.lSubType);

	if (subType == SPrimObject::SUBTYPE::SUBTYPE_STANDARD || subType == SPrimObject::SUBTYPE::SUBTYPE_LINKED)
	{
		//vertexBufferSize = primSubMesh.lNumVertices * 72;
		vertexBufferSize = primSubMesh.lNumVertices * (sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector2));
	}
	else
	{
		vertexBufferSize = primSubMesh.lNumVertices * 80;
	}

	vertexBuffer = operator new(vertexBufferSize);

	BinaryWriter binaryWriter = BinaryWriter(vertexBuffer, vertexBufferSize);

	for (unsigned int i = 0; i < primSubMesh.lNumVertices; ++i)
	{
		//Either write 1.0f for W coordinate or just write X, Y and Z since W coordinates can be 0 in some PRIM resources and when it's 0 model can't be rendered
		binaryWriter.Write(vertices[i].position.x);
		binaryWriter.Write(vertices[i].position.y);
		binaryWriter.Write(vertices[i].position.z);
		//binaryWriter.Write(1.0f);

		/*binaryWriter.Write(vertices[i].normal);
		binaryWriter.Write(vertices[i].tangent);
		binaryWriter.Write(vertices[i].binormal);*/
		binaryWriter.Write(vertices[i].normal.x);
		binaryWriter.Write(vertices[i].normal.y);
		binaryWriter.Write(vertices[i].normal.z);

		binaryWriter.Write(vertices[i].tangent.x);
		binaryWriter.Write(vertices[i].tangent.y);
		binaryWriter.Write(vertices[i].tangent.z);

		binaryWriter.Write(vertices[i].binormal.x);
		binaryWriter.Write(vertices[i].binormal.y);
		binaryWriter.Write(vertices[i].binormal.z);

		binaryWriter.Write(vertices[i].uv[0]);

		if (subType == SPrimObject::SUBTYPE::SUBTYPE_WEIGHTED)
		{
			for (unsigned int j = 0; j < 4; ++j)
			{
				binaryWriter.Write(vertices[i].weights[j]);
			}
		}
	}
}

void RenderPrimitive::StandardMesh::Deserialize(BinaryReader& binaryReader, const bool hasHighResolutionPositions)
{
	primMesh = binaryReader.Read<SPrimMesh>();

	binaryReader.Seek(primMesh.lSubMeshTable, SeekOrigin::Begin);

	const unsigned int subMeshOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(subMeshOffset, SeekOrigin::Begin);

	primSubMesh = binaryReader.Read<SPrimSubMesh>();

	if (primSubMesh.lNumUVChannels == 0) //lNumUVChannels is 0 in some PRIMs that have UV coordinates
	{
		primSubMesh.lNumUVChannels = 1;
	}

	binaryReader.Seek(primSubMesh.lIndices, SeekOrigin::Begin);

	ReadIndices(binaryReader);
	ReadVertices(binaryReader, hasHighResolutionPositions);
}

void RenderPrimitive::StandardMesh::ReadVertices(BinaryReader& binaryReader, const bool hasHighResolutionPositions)
{
	vertices.resize(primSubMesh.lNumVertices);

	binaryReader.Seek(primSubMesh.lVertices, SeekOrigin::Begin);

	for (unsigned int i = 0; i < primSubMesh.lNumVertices; ++i)
	{
		ReadVertexPosition(binaryReader, i, hasHighResolutionPositions, primMesh.vPosScale, primMesh.vPosBias);
		ReadVertexNormal(binaryReader, i);
		ReadVertexColor(binaryReader, i);
		ReadVertexTangent(binaryReader, i);
		ReadVertexBinormal(binaryReader, i);

		for (unsigned char uv = 0; uv < primSubMesh.lNumUVChannels; ++uv)
		{
			ReadVertexUVs(binaryReader, i, primMesh.vTexScale, primMesh.vTexBias);
		}
	}
}

const bool RenderPrimitive::StandardMesh::IsWeighted() const
{
	return false;
}

const unsigned short RenderPrimitive::StandardMesh::GetMaterialID() const
{
	return primMesh.lMaterialId;
}

const unsigned char RenderPrimitive::StandardMesh::GetLODMask() const
{
	return primMesh.lLODMask;
}

void RenderPrimitive::LinkedMesh::Deserialize(BinaryReader& binaryReader, const bool hasHighResolutionPositions)
{
	primMeshWeighted = binaryReader.Read<SPrimMeshWeighted>();

	binaryReader.Seek(primMeshWeighted.lSubMeshTable, SeekOrigin::Begin);

	const unsigned int subMeshOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(subMeshOffset, SeekOrigin::Begin);

	primSubMesh = binaryReader.Read<SPrimSubMesh>();

	if (primSubMesh.lNumUVChannels == 0) //lNumUVChannels is 0 in some PRIMs that have UV coordinates
	{
		primSubMesh.lNumUVChannels = 1;
	}

	binaryReader.Seek(primSubMesh.lIndices, SeekOrigin::Begin);

	ReadIndices(binaryReader);
	ReadVertices(binaryReader, hasHighResolutionPositions);

	binaryReader.Seek(primMeshWeighted.lBoneInfo, SeekOrigin::Begin);

	boneInfo = binaryReader.Read<SBoneInfo>();
}

void RenderPrimitive::LinkedMesh::ReadVertices(BinaryReader& binaryReader, const bool hasHighResolutionPositions)
{
	vertices.resize(primSubMesh.lNumVertices);

	binaryReader.Seek(primSubMesh.lVertices, SeekOrigin::Begin);

	for (unsigned int i = 0; i < primSubMesh.lNumVertices; ++i)
	{
		ReadVertexPosition(binaryReader, i, hasHighResolutionPositions, primMeshWeighted.vPosScale, primMeshWeighted.vPosBias);
	}

	for (unsigned int i = 0; i < primSubMesh.lNumVertices; ++i)
	{
		ReadVertexNormal(binaryReader, i);
		ReadVertexTangent(binaryReader, i);
		ReadVertexBinormal(binaryReader, i);

		for (unsigned char uv = 0; uv < primSubMesh.lNumUVChannels; ++uv)
		{
			ReadVertexUVs(binaryReader, i, primMeshWeighted.vTexScale, primMeshWeighted.vTexBias);
		}

		ReadVertexColor(binaryReader, i);
	}
}

const bool RenderPrimitive::LinkedMesh::IsWeighted() const
{
	return false;
}

const unsigned short RenderPrimitive::LinkedMesh::GetMaterialID() const
{
	return primMeshWeighted.lMaterialId;
}

const unsigned char RenderPrimitive::LinkedMesh::GetLODMask() const
{
	return primMeshWeighted.lLODMask;
}

void RenderPrimitive::WeightedMesh::Deserialize(BinaryReader& binaryReader, const bool hasHighResolutionPositions)
{
	primMeshWeighted = binaryReader.Read<SPrimMeshWeighted>();

	binaryReader.Seek(primMeshWeighted.lSubMeshTable, SeekOrigin::Begin);

	const unsigned int subMeshOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(subMeshOffset, SeekOrigin::Begin);

	primSubMesh = binaryReader.Read<SPrimSubMesh>();

	if (primSubMesh.lNumUVChannels == 0) //lNumUVChannels is 0 in some PRIMs that have UV coordinates
	{
		primSubMesh.lNumUVChannels = 1;
	}

	binaryReader.Seek(primSubMesh.lIndices, SeekOrigin::Begin);

	ReadIndices(binaryReader);
	ReadVertices(binaryReader, hasHighResolutionPositions);

	binaryReader.Seek(primMeshWeighted.lBoneInfo, SeekOrigin::Begin);

	boneInfo = binaryReader.Read<SBoneInfo>();
}

void RenderPrimitive::WeightedMesh::ReadVertices(BinaryReader& binaryReader, const bool hasHighResolutionPositions)
{
	vertices.resize(primSubMesh.lNumVertices);

	binaryReader.Seek(primSubMesh.lVertices, SeekOrigin::Begin);

	for (unsigned int i = 0; i < primSubMesh.lNumVertices; ++i)
	{
		ReadVertexPosition(binaryReader, i, hasHighResolutionPositions, primMeshWeighted.vPosScale, primMeshWeighted.vPosBias);
	}

	for (unsigned int i = 0; i < primSubMesh.lNumVertices; ++i)
	{
		ReadVertexWeighsAndBoneRemapValues(binaryReader, i);
	}

	for (unsigned int i = 0; i < primSubMesh.lNumVertices; ++i)
	{
		ReadVertexNormal(binaryReader, i);
		ReadVertexTangent(binaryReader, i);
		ReadVertexBinormal(binaryReader, i);

		for (unsigned char uv = 0; uv < primSubMesh.lNumUVChannels; ++uv)
		{
			ReadVertexUVs(binaryReader, i, primMeshWeighted.vTexScale, primMeshWeighted.vTexBias);
		}

		ReadVertexColor(binaryReader, i);
	}
}

const bool RenderPrimitive::WeightedMesh::IsWeighted() const
{
	return static_cast<SPrimObject::SUBTYPE>(primMeshWeighted.lSubType) == SPrimObject::SUBTYPE::SUBTYPE_WEIGHTED;
}

const unsigned short RenderPrimitive::WeightedMesh::GetMaterialID() const
{
	return primMeshWeighted.lMaterialId;
}

const unsigned char RenderPrimitive::WeightedMesh::GetLODMask() const
{
	return primMeshWeighted.lLODMask;
}

const unsigned char RenderPrimitive::WeightedMesh::GetBoneIndex(unsigned char boneRemapValue) const
{
	for (unsigned char i = 0; i < 255; ++i)
	{
		if (boneInfo.aBoneRemap[i] == boneRemapValue)
		{
			return i;
		}
	}

	return -1;
}

void RenderPrimitive::ConvertToOBJ(const std::string& objFilePath)
{
	/*std::ofstream ofstream = std::ofstream(objFilePath, std::ofstream::binary);

	if (!ofstream.good())
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Error: OBJ file {} could not be created.", objFilePath);

		return;
	}*/

	std::ofstream objFile = std::ofstream(std::format("{}.obj", GetName()));
	unsigned int vertexCount = 1;

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		unsigned char lodMask = meshes[i]->GetLODMask();

		if ((lodMask & 1) != 1)
		{
			continue;
		}

		std::vector<Vertex>& vertices = meshes[i]->GetVertices();

		//objFile << "o Mesh " << i << std::endl;

		for (size_t j = 0; j < vertices.size(); ++j)
		{
			objFile << std::format("v {} {} {}\n", vertices[j].position.x, vertices[j].position.y, vertices[j].position.z);
			//objFile << std::format("v {} {} {}\n", vertices[j].position.x, vertices[j].position.z, -vertices[j].position.y);
		}

		/*for (size_t j = 0; j < vertices.size(); ++j)
		{
			objFile << std::format("vn {} {} {}\n", vertices[j].normal.x, vertices[j].normal.y, vertices[j].normal.z);
		}*/

		if (vertices[0].uv.size() > 0)
		{
			for (size_t j = 0; j < vertices.size(); ++j)
			{
				std::vector<Vector2>& uv = vertices[j].uv;

				objFile << std::format("vt {} {}\n", uv[0].x, -1 * uv[0].y + 1);
			}

			std::vector<std::shared_ptr<Resource>>& primReferences = GetReferences();
			const unsigned int matiReferenceIndex = meshes[i]->GetMaterialID();
			std::shared_ptr<Resource> matiReference = primReferences[matiReferenceIndex];

			const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(matiReference->GetHash());
			RenderMaterialInstance renderMaterialInstance;
			std::vector<RenderMaterialInstance::Texture> textures;

			matiReference->SetHeaderLibraries(&referenceInfo.headerLibraries);
			matiReference->LoadResource(0, referenceInfo.headerLibraries[0].chunkIndex, referenceInfo.headerLibraries[0].indexInLibrary, true, false, true);

			if (matiReference->GetRuntimeResourceID().GetID() == 0x80000049E33E805D)
			{
				int a = 2;
			}

			renderMaterialInstance.Deserialize(matiReference->GetResourceData(), matiReference->GetResourceDataSize());
			renderMaterialInstance.GetTextures(matiReference, textures);

			std::string materialResourceName = ResourceUtility::GetResourceName(matiReference->GetResourceID());
			std::vector<std::shared_ptr<Resource>>& matiReferences = matiReference->GetReferences();

			for (size_t j = 0; j < textures.size(); ++j)
			{
				unsigned int textureReferenceIndex = textures[j].textureReferenceIndex;
				std::string textureResourceID = matiReferences[textureReferenceIndex]->GetResourceID();
				std::string textureResourceName = ResourceUtility::GetResourceName(textureResourceID);
				std::shared_ptr<Resource> textReference = matiReferences[textures[j].textureReferenceIndex];
				const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(textReference->GetHash());
				Texture texture;

				textures[j].name = std::format("{}_{}.png", textureResourceName, matiReferenceIndex);

				textReference->SetHeaderLibraries(&referenceInfo.headerLibraries);
				textReference->LoadResource(0, referenceInfo.headerLibraries[0].chunkIndex, referenceInfo.headerLibraries[0].indexInLibrary, false, false, true);
				texture.Deserialize(textReference->GetResourceData(), textReference->GetResourceDataSize());
				texture.ConvertTextureToPNG(textures[j].name);
				textReference->DeleteResourceData();
			}

			matiReference->DeleteResourceData();

			std::string mtlFileName = std::format("{}.mtl", materialResourceName);
			std::ofstream mtlFile = std::ofstream(mtlFileName);

			objFile << "mtllib " << mtlFileName << std::endl;

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

			objFile << "usemtl " << materialResourceName << std::endl;
			objFile << "s 1" << std::endl;

			const std::vector<unsigned short>& indices = meshes[i]->GetIndices();

			for (size_t j = 0; j < indices.size() / 3; ++j)
			{
				unsigned int vertexIndex = indices[j * 3] + vertexCount;
				unsigned int vertexIndex2 = indices[j * 3 + 1] + vertexCount;
				unsigned int vertexIndex3 = indices[j * 3 + 2] + vertexCount;

				objFile << std::format("f {}/{} {}/{} {}/{}\n", vertexIndex, vertexIndex, vertexIndex2, vertexIndex2, vertexIndex3, vertexIndex3);
			}

			mtlFile.close();
		}
		else
		{
			objFile << "s 1" << std::endl;

			const std::vector<unsigned short>& indices = meshes[i]->GetIndices();

			for (size_t j = 0; j < indices.size() / 3; ++j)
			{
				unsigned int vertexIndex = indices[j * 3] + vertexCount;
				unsigned int vertexIndex2 = indices[j * 3 + 1] + vertexCount;
				unsigned int vertexIndex3 = indices[j * 3 + 2] + vertexCount;

				objFile << std::format("f {} {} {}\n", vertexIndex, vertexIndex2, vertexIndex3);
			}
		}

		vertexCount += meshes[i]->GetVertices().size();
	}

	objFile.close();
}

void RenderPrimitive::ConvertToGLB(const std::string& resourceName, const std::string& gltfFilePath, bool rotate)
{
	std::vector<std::vector<float>> verticesMin;
	std::vector<std::vector<float>> verticesMax;

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		std::vector<float> tempVerticesMin;
		std::vector<float> tempVerticesMax;
		unsigned char lodMask = meshes[i]->GetLODMask();

		if ((lodMask & 1) != 1)
		{
			verticesMin.push_back(tempVerticesMin);
			verticesMax.push_back(tempVerticesMax);

			continue;
		}

		std::vector<Vertex> vertices = meshes[i]->GetVertices();

		for (size_t j = 0; j < vertices.size(); j++)
		{
			if (j == 0)
			{
				tempVerticesMin.push_back(vertices[j].position.x);
				tempVerticesMin.push_back(vertices[j].position.y);
				tempVerticesMin.push_back(vertices[j].position.z);

				tempVerticesMax.push_back(vertices[j].position.x);
				tempVerticesMax.push_back(vertices[j].position.y);
				tempVerticesMax.push_back(vertices[j].position.x);
			}
			else
			{
				if (vertices[j].position.x < tempVerticesMin[0])
				{
					tempVerticesMin[0] = vertices[j].position.x;
				}

				if (vertices[j].position.x > tempVerticesMax[0])
				{
					tempVerticesMax[0] = vertices[j].position.x;
				}

				if (vertices[j].position.y < tempVerticesMin[1])
				{
					tempVerticesMin[1] = vertices[j].position.y;
				}

				if (vertices[j].position.y > tempVerticesMax[1])
				{
					tempVerticesMax[1] = vertices[j].position.y;
				}

				if (vertices[j].position.z < tempVerticesMin[2])
				{
					tempVerticesMin[2] = vertices[j].position.z;
				}

				if (vertices[j].position.z > tempVerticesMax[2])
				{
					tempVerticesMax[2] = vertices[j].position.z;
				}
			}
		}

		verticesMin.push_back(tempVerticesMin);
		verticesMax.push_back(tempVerticesMax);
	}

	std::filesystem::path gltfFilePath2 = gltfFilePath;
	std::filesystem::path glbFileName = gltfFilePath2.filename();

	std::unique_ptr<GLTFStreamWriter> streamWriter = std::make_unique<GLTFStreamWriter>(gltfFilePath2.parent_path().string());
	std::unique_ptr<Microsoft::glTF::ResourceWriter> resourceWriter = std::make_unique<Microsoft::glTF::GLBResourceWriter>(std::move(streamWriter));

	if (!resourceWriter)
	{
		throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
	}

	Microsoft::glTF::Document document;
	Microsoft::glTF::BufferBuilder bufferBuilder(std::move(resourceWriter));

	bufferBuilder.AddBuffer(Microsoft::glTF::GLB_BUFFER_ID);

	std::unordered_map<unsigned int, std::string> materialIDs;
	std::vector<std::shared_ptr<Resource>>& primReferences = GetReferences();

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		unsigned char lodMask = meshes[i]->GetLODMask();

		if ((lodMask & 1) != 1)
		{
			continue;
		}

		const unsigned int matiReferenceIndex = meshes[i]->GetMaterialID();
		std::shared_ptr<Resource> matiReference = primReferences[matiReferenceIndex];

		const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(matiReference->GetHash());
		RenderMaterialInstance renderMaterialInstance;
		std::vector<RenderMaterialInstance::Texture> textures;

		matiReference->SetHeaderLibraries(&referenceInfo.headerLibraries);
		matiReference->LoadResource(0, referenceInfo.headerLibraries[0].chunkIndex, referenceInfo.headerLibraries[0].indexInLibrary, true, false, true);

		renderMaterialInstance.Deserialize(matiReference->GetResourceData(), matiReference->GetResourceDataSize());
		renderMaterialInstance.GetTextures(matiReference, textures);

		std::vector<std::shared_ptr<Resource>>& matiReferences = matiReference->GetReferences();

		for (size_t j = 0; j < textures.size(); ++j)
		{
			unsigned int textureReferenceIndex = textures[j].textureReferenceIndex;
			std::string textureResourceID = matiReferences[textureReferenceIndex]->GetResourceID();
			std::string textureResourceName = ResourceUtility::GetResourceName(textureResourceID);
			std::shared_ptr<Resource> textReference = matiReferences[textures[j].textureReferenceIndex];
			const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(textReference->GetHash());
			Texture texture;
			DirectX::Blob blob;

			textReference->SetHeaderLibraries(&referenceInfo.headerLibraries);
			textReference->LoadResource(0, referenceInfo.headerLibraries[0].chunkIndex, referenceInfo.headerLibraries[0].indexInLibrary, false, false, true);
			texture.Deserialize(textReference->GetResourceData(), textReference->GetResourceDataSize());
			texture.ConvertTextureToPNG(&blob);

			Microsoft::glTF::Material material;

			if (textures[j].type == RenderMaterialInstance::Texture::Type::Normal)
			{
				Microsoft::glTF::Image image;
				Microsoft::glTF::Texture gltfTexture;

				image.bufferViewId = bufferBuilder.AddBufferView(blob.GetBufferPointer(), blob.GetBufferSize()).id;
				image.mimeType = "image/png";

				std::string imageID = document.images.Append(std::move(image), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

				gltfTexture.imageId = imageID;

				std::string textureID = document.textures.Append(std::move(gltfTexture), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

				material.normalTexture.textureId = textureID;
			}
			else if (textures[j].type == RenderMaterialInstance::Texture::Type::Diffuse)
			{
				Microsoft::glTF::Image image;
				Microsoft::glTF::Texture gltfTexture;

				image.bufferViewId = bufferBuilder.AddBufferView(blob.GetBufferPointer(), blob.GetBufferSize()).id;
				image.mimeType = "image/png";

				std::string imageID = document.images.Append(std::move(image), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

				gltfTexture.imageId = imageID;

				std::string textureID = document.textures.Append(std::move(gltfTexture), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

				material.metallicRoughness.baseColorTexture.textureId = textureID;
			}
			else if (textures[j].type == RenderMaterialInstance::Texture::Type::Emissive)
			{
				Microsoft::glTF::Image image;
				Microsoft::glTF::Texture gltfTexture;

				image.bufferViewId = bufferBuilder.AddBufferView(blob.GetBufferPointer(), blob.GetBufferSize()).id;
				image.mimeType = "image/png";

				std::string imageID = document.images.Append(std::move(image), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

				gltfTexture.imageId = imageID;

				std::string textureID = document.textures.Append(std::move(gltfTexture), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

				material.emissiveTexture.textureId = textureID;
			}

			material.metallicRoughness.metallicFactor = 0.0f;
			material.metallicRoughness.roughnessFactor = 0.0f;

			materialIDs[meshes[i]->GetMaterialID()] = document.materials.Append(std::move(material), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

			textReference->DeleteResourceData();
		}

		matiReference->DeleteResourceData();
	}

	std::vector<std::string> nodeIDs;
	bool areBonesAdded = false;
	bool isAnyMeshWeighted = false;
	std::vector<std::string> joints2;
	std::string skinID = "";

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		unsigned char lodMask = meshes[i]->GetLODMask();

		if ((lodMask & 1) != 1)
		{
			continue;
		}

		if (meshes[i]->IsWeighted())
		{
			isAnyMeshWeighted = true;

			break;
		}
	}

	if (!isAnyMeshWeighted)
	{
		Microsoft::glTF::Node node;

		node.name = resourceName;

		if (rotate)
		{
			node.rotation = Microsoft::glTF::Quaternion::Quaternion(static_cast<float>(Math::Sqrt(2.0) / 2.0), 0.f, 0.f, static_cast<float>(-Math::Sqrt(2.0) / 2.0));
		}

		for (uint32_t b = 1; b < (meshes.size() + 1); b++)
		{
			node.children.push_back(std::to_string(b));
		}

		document.nodes.Append(std::move(node), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty);
	}

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		unsigned char lodMask = meshes[i]->GetLODMask();

		if ((lodMask & 1) != 1)
		{
			continue;
		}

		Microsoft::glTF::MeshPrimitive meshPrimitive;

		bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ELEMENT_ARRAY_BUFFER);
		meshPrimitive.indicesAccessorId = bufferBuilder.AddAccessor(meshes[i]->GetIndices(),
			{ Microsoft::glTF::AccessorType::TYPE_SCALAR, Microsoft::glTF::COMPONENT_UNSIGNED_SHORT }).id;

		bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);
		meshPrimitive.attributes["POSITION"] = bufferBuilder.AddAccessor(meshes[i]->GetPositions(),
			{ Microsoft::glTF::TYPE_VEC3, Microsoft::glTF::COMPONENT_FLOAT, false,
			 std::move(verticesMin[i]),
			 std::move(verticesMax[i]) }).id;

		bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);
		meshPrimitive.attributes["NORMAL"] = bufferBuilder.AddAccessor(meshes[i]->GetNormals(),
			{ Microsoft::glTF::TYPE_VEC3, Microsoft::glTF::COMPONENT_FLOAT }).id;

		std::vector<std::vector<float>> uvs = meshes[i]->GetUVs();

		for (size_t uv = 0; uv < uvs.size(); uv++)
		{
			std::string label = ("TEXCOORD_" + std::to_string(unsigned(uv)));

			bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);
			meshPrimitive.attributes[label] = bufferBuilder.AddAccessor(uvs[uv],
				{ Microsoft::glTF::TYPE_VEC2, Microsoft::glTF::COMPONENT_FLOAT }).id;
		}

		if (meshes[i]->IsWeighted())
		{
			std::vector<float> weights = meshes[i]->GetWeights();
			std::vector<unsigned char> boneRemapValues = meshes[i]->GetBoneRemapValues();
			std::vector<unsigned char> joints;

			joints.reserve(boneRemapValues.size() * 4);

			for (size_t j = 0; j < boneRemapValues.size(); j += 4)
			{
				if (weights[j] != 0)
				{
					joints.push_back(static_cast<WeightedMesh*>(meshes[i])->GetBoneIndex(boneRemapValues[j] / 3));
				}
				else
				{
					joints.push_back(boneRemapValues[j]);
				}

				if (weights[j + 1] != 0)
				{
					joints.push_back(static_cast<WeightedMesh*>(meshes[i])->GetBoneIndex(boneRemapValues[j + 1] / 3));
				}
				else
				{
					joints.push_back(boneRemapValues[j + 1]);
				}

				if (weights[j + 2] != 0)
				{
					joints.push_back(static_cast<WeightedMesh*>(meshes[i])->GetBoneIndex(boneRemapValues[j + 2] / 3));
				}
				else
				{
					joints.push_back(boneRemapValues[j + 2]);
				}

				if (weights[j + 3] != 0)
				{
					joints.push_back(static_cast<WeightedMesh*>(meshes[i])->GetBoneIndex(boneRemapValues[j + 3] / 3));
				}
				else
				{
					joints.push_back(boneRemapValues[j + 3]);
				}
			}

			bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);
			meshPrimitive.attributes["JOINTS_0"] = bufferBuilder.AddAccessor(
				joints, { Microsoft::glTF::AccessorType::TYPE_VEC4, Microsoft::glTF::ComponentType::COMPONENT_UNSIGNED_BYTE }).id;

			bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);
			meshPrimitive.attributes["WEIGHTS_0"] = bufferBuilder.AddAccessor(
				weights, { Microsoft::glTF::AccessorType::TYPE_VEC4, Microsoft::glTF::ComponentType::COMPONENT_FLOAT }).id;

			if (!areBonesAdded)
			{
				areBonesAdded = true;

				std::vector<Microsoft::glTF::Node> bones;
				const std::vector<SBoneDefinition> boneDefinitions = boneRig->GetBoneDefinitions();
				const std::vector<SQV> bindPoses = boneRig->GetBindPoses();

				for (uint32_t b = 0; b < boneDefinitions.size(); b++)
				{
					bones.emplace_back();

					bones.back().name = boneDefinitions[b].Name;
					bones.back().translation = Microsoft::glTF::Vector3::Vector3(bindPoses[b].m_Translation.m.m128_f32[0],
						bindPoses[b].m_Translation.m.m128_f32[1], bindPoses[b].m_Translation.m.m128_f32[2]);
					bones.back().rotation = Microsoft::glTF::Quaternion::Quaternion(-bindPoses[b].m_Rotation.w128.m.m128_f32[0],
						-bindPoses[b].m_Rotation.w128.m.m128_f32[1],
						-bindPoses[b].m_Rotation.w128.m.m128_f32[2],
						bindPoses[b].m_Rotation.w128.m.m128_f32[3]);
				}

				uint32_t nodeIDOffset = document.nodes.Size();

				for (uint32_t b = 0; b < boneDefinitions.size(); b++)
				{
					for (uint32_t a = 0; a < boneDefinitions.size(); a++)
					{
						if (b == boneDefinitions[a].lPrevBoneNr)
						{
							bones.at(b).children.push_back(std::to_string(a + nodeIDOffset));
						}
					}
				}

				for (uint32_t b = 0; b < boneDefinitions.size(); b++)
				{
					joints2.emplace_back(document.nodes.Append(bones.at(b), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id);
				}

				Microsoft::glTF::Node node;

				node.name = resourceName;
				node.children.push_back(joints2.front());

				if (rotate)
				{
					node.rotation = Microsoft::glTF::Quaternion::Quaternion(static_cast<float>(Math::Sqrt(2.0) / 2.0), 0.f, 0.f, static_cast<float>(-Math::Sqrt(2.0) / 2.0));
				}

				nodeIDs.push_back(document.nodes.Append(std::move(node), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id);

				Microsoft::glTF::Skin tempSkin;

				tempSkin.name = "S_" + resourceName;

				for (const auto& joint : joints2)
				{
					tempSkin.jointIds.push_back(joint);
				}

				std::vector<SMatrix44> bindPoseInverseGlobalMatrices = boneRig->GetBindPoseInverseGlobalMatrices();
				std::vector<float> bindPoseInverseGlobalMatrices2;

				bindPoseInverseGlobalMatrices2.reserve(bindPoseInverseGlobalMatrices.size() * 4);

				for (unsigned int j = 0; j < bindPoseInverseGlobalMatrices.size(); ++j)
				{
					for (unsigned int k = 0; k < 4; ++k)
					{
						bindPoseInverseGlobalMatrices2.push_back(bindPoseInverseGlobalMatrices[j].r[k].x);
						bindPoseInverseGlobalMatrices2.push_back(bindPoseInverseGlobalMatrices[j].r[k].y);
						bindPoseInverseGlobalMatrices2.push_back(bindPoseInverseGlobalMatrices[j].r[k].z);
						bindPoseInverseGlobalMatrices2.push_back(bindPoseInverseGlobalMatrices[j].r[k].w);
					}
				}

				bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);

				tempSkin.inverseBindMatricesAccessorId = bufferBuilder.AddAccessor(
					bindPoseInverseGlobalMatrices2, { Microsoft::glTF::TYPE_MAT4, Microsoft::glTF::COMPONENT_FLOAT }).id;

				tempSkin.skeletonId = joints2.front();

				skinID = document.skins.Append(std::move(tempSkin), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;
			}
		}

		auto it = materialIDs.find(meshes[i]->GetMaterialID());

		if (it != materialIDs.end())
		{
			meshPrimitive.materialId = it->second;
		}

		Microsoft::glTF::Mesh mesh;
		mesh.primitives.push_back(std::move(meshPrimitive));
		mesh.name = std::format("{}_{}", resourceName, i);

		auto meshId = document.meshes.Append(std::move(mesh), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

		Microsoft::glTF::Node node;
		node.meshId = meshId;

		if (meshes[i]->IsWeighted())
		{
			node.skinId = skinID;
		}

		nodeIDs.push_back(document.nodes.Append(std::move(node), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id);
	}

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
