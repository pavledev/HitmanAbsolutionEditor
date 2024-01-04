#pragma once

#include <vector>

#include "Glacier/RenderPrimitive/SPrimObjectHeader.h"
#include "Glacier/RenderPrimitive/SPrimMesh.h"
#include "Glacier/RenderPrimitive/SPrimSubMesh.h"
#include "Glacier/RenderPrimitive/SPrimMeshWeighted.h"

#include "IO/BinaryReader.h"
#include "Resources/BoneRig.h"
#include "Resources/RenderMaterialInstance.h"
#include "Glacier/RenderPrimitive/SBoneInfo.h"
#include "Physics.h"

class RenderPrimitive : public Resource
{
public:
	struct ColorRGBA
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	struct Vertex
	{
		Vector4 position;
		Vector4 normal;
		Vector4 tangent;
		Vector4 binormal;
		std::vector<Vector2> uv;
		ColorRGBA color;
		float weights[4];
		unsigned char boneRemapValues[4];
	};

	class Mesh
	{
	public:
		Mesh();
		~Mesh();
		virtual void Deserialize(BinaryReader& binaryReader, const bool hasHighResolutionPositions) = 0;
		void ReadIndices(BinaryReader& binaryReader);
		virtual void ReadVertices(BinaryReader& binaryReader, const bool hasHighResolutionPositions) = 0;
		void ReadVertexPosition(BinaryReader& binaryReader, const unsigned int vertexIndex, const bool hasHighResolutionPositions, const Vector4& scale, const Vector4& bias);
		void ReadVertexWeighsAndBoneRemapValues(BinaryReader& binaryReader, const unsigned int vertexIndex);
		void ReadVertexNormal(BinaryReader& binaryReader, const unsigned int vertexIndex);
		void ReadVertexTangent(BinaryReader& binaryReader, const unsigned int vertexIndex);
		void ReadVertexBinormal(BinaryReader& binaryReader, const unsigned int vertexIndex);
		void ReadVertexUVs(BinaryReader& binaryReader, const unsigned int vertexIndex, const Vector2& scale, const Vector2& bias);
		void ReadVertexColor(BinaryReader& binaryReader, const unsigned int vertexIndex);
		const SPrimObject::SUBTYPE GetSubType() const;
		const unsigned int GetIndexCount() const;
		const unsigned int GetVertexCount() const;
		const std::vector<unsigned short>& GetIndices() const;
		const std::vector<Vertex>& GetVertices() const;
		std::vector<Vertex>& GetVertices();
		const std::vector<float> GetPositions() const;
		const std::vector<float> GetNormals() const;
		const std::vector<std::vector<float>> GetUVs() const;
		const std::vector<float> GetWeights() const;
		const std::vector<unsigned char> GetBoneRemapValues() const;
		const std::vector<unsigned char> GetColors() const;
		const Vector3 GetBoundingBoxMin() const;
		const Vector3 GetBoundingBoxMax() const;
		virtual const bool IsWeighted() const = 0;
		virtual const unsigned short GetMaterialID() const = 0;
		virtual const unsigned char GetLODMask() const = 0;
		const void* GetVertexBuffer() const;
		const unsigned int GetVertexBufferSize() const;
		const unsigned int GetStride() const;
		void CreateVertexBuffer();

	protected:
		SPrimSubMesh primSubMesh;
		std::vector<unsigned short> indices;
		std::vector<Vertex> vertices;
		void* vertexBuffer;
		unsigned int vertexBufferSize;
	};

	class StandardMesh : public Mesh
	{
	public:
		void Deserialize(BinaryReader& binaryReader, const bool hasHighResolutionPositions) override;
		void ReadVertices(BinaryReader& binaryReader, const bool hasHighResolutionPositions) override;
		const bool IsWeighted() const override;
		const unsigned short GetMaterialID() const override;
		const unsigned char GetLODMask() const override;

	private:
		SPrimMesh primMesh;
	};

	class LinkedMesh : public Mesh
	{
	public:
		void Deserialize(BinaryReader& binaryReader, const bool hasHighResolutionPositions) override;
		void ReadVertices(BinaryReader& binaryReader, const bool hasHighResolutionPositions) override;
		const bool IsWeighted() const override;
		const unsigned short GetMaterialID() const override;
		const unsigned char GetLODMask() const override;

	private:
		SPrimMeshWeighted primMeshWeighted;
		SBoneInfo boneInfo;
	};

	class WeightedMesh : public Mesh
	{
	public:
		void Deserialize(BinaryReader& binaryReader, const bool hasHighResolutionPositions) override;
		void ReadVertices(BinaryReader& binaryReader, const bool hasHighResolutionPositions) override;
		const bool IsWeighted() const override;
		const unsigned short GetMaterialID() const override;
		const unsigned char GetLODMask() const override;
		const unsigned char GetBoneIndex(unsigned char boneRemapValue) const;

	private:
		SPrimMeshWeighted primMeshWeighted;
		SBoneInfo boneInfo;
	};

	~RenderPrimitive();
	const SPrimObjectHeader& GetPrimObjectHeader() const;
	const unsigned int GetBoneRigResourceIndex() const;
	const std::vector<Mesh*>& GetMeshes() const;
	const BoneRig* GetBoneRig() const;
	const Physics* GetPhysics() const;
	BoneRig* GetBoneRig();
	void SetBoneRig(BoneRig* boneRig);
	void SetPhysics(Physics* physics);
	void Deserialize() override;
	void ConvertToOBJ(const std::string& objFilePath);
	void ConvertToGLB(const std::string& resourceName, const std::string& gltfFilePath, bool rotate);

private:
	SPrimObjectHeader primObjectHeader;
	std::vector<Mesh*> meshes;
	BoneRig* boneRig;
	Physics* physics;
};
