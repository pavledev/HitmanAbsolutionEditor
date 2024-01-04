#include "Glacier/Serializer/ZBinaryDeserializer.h"

#include "Resources/BehaviorTreeEntityBlueprint.h"
#include "Utility/ResourceUtility.h"

void BehaviorTreeEntityBlueprint::Deserialize()
{
	ZBinaryDeserializer binaryDeserializer;
	void* behaviorTreeInfo = binaryDeserializer.Deserialize(resourceData, resourceDataSize);

	Parse(behaviorTreeInfo);

	operator delete(behaviorTreeInfo, std::align_val_t(binaryDeserializer.GetAlignment()));

	isResourceDeserialized = true;
}

void BehaviorTreeEntityBlueprint::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		behaviorTreeInfo->SerializeToJson(outputPath);
	}
}

void BehaviorTreeEntityBlueprint::Parse(void* behaviorTreeInfo)
{
	this->behaviorTreeInfo = std::make_shared<SBehaviorTreeInfo>();

	const unsigned int referencesStartAddress = *reinterpret_cast<unsigned int*>(behaviorTreeInfo);
	const unsigned int referencesEndAddress = *(reinterpret_cast<unsigned int*>(behaviorTreeInfo) + 1);
	const unsigned int referenceCount = ResourceUtility::CalculateArrayElementsCount(referencesStartAddress, referencesEndAddress, 0xC); //0xC is size of SBehaviorTreeEntityReference

	this->behaviorTreeInfo->m_references.Resize(referenceCount);

	for (unsigned int i = 0; i < referenceCount; i++)
	{
		const unsigned int referenceAddress = referencesStartAddress + 0xC * i;
		SBehaviorTreeEntityReference* behaviorTreeEntityReference = ResourceUtility::Convert4ByteAddressTo8BytePointer<SBehaviorTreeEntityReference>(behaviorTreeInfo, referenceAddress);

		this->behaviorTreeInfo->m_references[i].m_bList = *reinterpret_cast<bool*>(behaviorTreeEntityReference);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(behaviorTreeEntityReference) + 2);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(behaviorTreeInfo, charsAddress);

		this->behaviorTreeInfo->m_references[i].m_sName = ZString(chars);
	}

	const unsigned int inputPinConditionsStartAddress = *(reinterpret_cast<unsigned int*>(behaviorTreeInfo) + 3);
	const unsigned int inputPinConditionsEndAddress = *(reinterpret_cast<unsigned int*>(behaviorTreeInfo) + 4);
	const unsigned int inputPinConditionCount = ResourceUtility::CalculateArrayElementsCount(inputPinConditionsStartAddress, inputPinConditionsEndAddress, 0x8); //0x8 is size of SBehaviorTreeInputPinCondition

	this->behaviorTreeInfo->m_inputPinConditions.Resize(inputPinConditionCount);

	for (unsigned int i = 0; i < inputPinConditionCount; i++)
	{
		const unsigned int inputPinConditionAddress = inputPinConditionsStartAddress + 0x8 * i;
		SBehaviorTreeInputPinCondition* behaviorTreeInputPinCondition = ResourceUtility::Convert4ByteAddressTo8BytePointer<SBehaviorTreeInputPinCondition>(behaviorTreeInfo, inputPinConditionAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(behaviorTreeInputPinCondition) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(behaviorTreeInfo, charsAddress);

		this->behaviorTreeInfo->m_inputPinConditions[i].m_sName = ZString(chars);
	}
}

std::shared_ptr<SBehaviorTreeInfo> BehaviorTreeEntityBlueprint::GetBehaviorTreeInfo()
{
	return behaviorTreeInfo;
}
