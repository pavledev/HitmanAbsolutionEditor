#include "Glacier/Serializer/ZBinaryDeserializer.h"

#include "Resources/CppEntityBlueprint.h"
#include "Utility/ResourceUtility.h"
#include "Registry/TypeRegistry.h"

void CppEntityBlueprint::Deserialize()
{
	ZBinaryDeserializer binaryDeserializer;
	void* cppEntityBlueprint = binaryDeserializer.Deserialize(GetResourceData(), GetResourceDataSize());

	Parse(cppEntityBlueprint);

	operator delete(cppEntityBlueprint, std::align_val_t(binaryDeserializer.GetAlignment()));
}

void CppEntityBlueprint::Parse(void* cppEntityBlueprint)
{
	this->cppEntityBlueprint = std::make_shared<SCppEntityBlueprint>();

	unsigned int typeIDAddress = *(reinterpret_cast<unsigned int*>(cppEntityBlueprint));
	STypeID* typeID = ResourceUtility::Convert4ByteAddressTo8BytePointer<STypeID>(cppEntityBlueprint, typeIDAddress);
	unsigned int typeAddress = reinterpret_cast<unsigned int>(typeID->pTypeInfo);
	IType* type = ResourceUtility::Convert4ByteAddressTo8BytePointer<IType>(cppEntityBlueprint, typeAddress);
	STypeID* typeID2 = TypeRegistry::GetInstance().GetTypeID(type->GetTypeName());

	this->cppEntityBlueprint->typeName = typeID2;

	const unsigned int subsetsStartAddress = *(reinterpret_cast<unsigned int*>(cppEntityBlueprint) + 1);
	const unsigned int subsetsEndAddress = *(reinterpret_cast<unsigned int*>(cppEntityBlueprint) + 2);
	const unsigned int subsetCount = ResourceUtility::CalculateArrayElementsCount(subsetsStartAddress, subsetsEndAddress, 0x10); //0x10 is size of SCppEntitySubsetInfo

	this->cppEntityBlueprint->subsets.Resize(subsetCount);

	for (unsigned int i = 0; i < subsetCount; i++)
	{
		const unsigned int subsetAddress = subsetsStartAddress + 0x10 * i;
		SCppEntitySubsetInfo* cppEntitySubsetInfo = ResourceUtility::Convert4ByteAddressTo8BytePointer<SCppEntitySubsetInfo>(cppEntityBlueprint, subsetAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(cppEntitySubsetInfo) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(cppEntityBlueprint, charsAddress);

		typeIDAddress = *(reinterpret_cast<unsigned int*>(cppEntitySubsetInfo) + 2);
		typeID = ResourceUtility::Convert4ByteAddressTo8BytePointer<STypeID>(cppEntityBlueprint, typeIDAddress);
		typeAddress = reinterpret_cast<unsigned int>(typeID->pTypeInfo);
		type = ResourceUtility::Convert4ByteAddressTo8BytePointer<IType>(cppEntityBlueprint, typeAddress);
		typeID2 = TypeRegistry::GetInstance().GetTypeID(type->GetTypeName());

		this->cppEntityBlueprint->subsets[i].name = ZString(chars);
		this->cppEntityBlueprint->subsets[i].type = typeID2;
		this->cppEntityBlueprint->subsets[i].flags = *(reinterpret_cast<unsigned int*>(cppEntitySubsetInfo) + 3);
	}
}

std::shared_ptr<SCppEntityBlueprint> CppEntityBlueprint::GetCppEntityBlueprint()
{
	return cppEntityBlueprint;
}
