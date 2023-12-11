#include <sstream>

#include "Glacier/Morpheme/Morpheme.h"

namespace NMP
{
	void StringTable::Locate()
	{
		m_IDs = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_IDs) + reinterpret_cast<uintptr_t>(this));
		m_Offsets = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_Offsets) + reinterpret_cast<uintptr_t>(this));
		m_Data += reinterpret_cast<uintptr_t>(this);
	}

	void StringTable::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		writer.String("m_NumEntrys");
		writer.Uint(m_NumEntrys);

		writer.String("m_DataLength");
		writer.Uint(m_DataLength);

		writer.String("m_IDs");
		writer.StartArray();

		for (unsigned int i = 0; i < m_NumEntrys; ++i)
		{
			writer.Uint(m_IDs[i]);
		}

		writer.EndArray();

		writer.String("m_Offsets");
		writer.StartArray();

		for (unsigned int i = 0; i < m_NumEntrys; ++i)
		{
			writer.Uint(m_Offsets[i]);
		}

		writer.EndArray();

		writer.String("m_Data");
		writer.StartArray();

		unsigned int length = 0;

		while (length < m_DataLength)
		{
			const char* name = m_Data + length;

			writer.String(m_Data + length);

			length += strlen(name) + 1;
		}

		writer.EndArray();

		writer.EndObject();
	}

	void Vector3::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		writer.String("x");
		writer.Double(x);

		writer.String("y");
		writer.Double(y);

		writer.String("z");
		writer.Double(z);

		writer.String("w");
		writer.Double(w);

		writer.EndObject();
	}

	void Quat::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		writer.String("x");
		writer.Double(x);

		writer.String("y");
		writer.Double(y);

		writer.String("z");
		writer.Double(z);

		writer.String("w");
		writer.Double(w);

		writer.EndObject();
	}

	void Hierarchy::Locate()
	{
		m_HierarchyArray = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(m_HierarchyArray) + reinterpret_cast<uintptr_t>(this));
	}

	void Hierarchy::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		writer.String("m_NumEntries");
		writer.Uint(m_NumEntries);

		writer.String("m_HierarchyArray");
		writer.StartArray();

		for (unsigned int i = 0; i < m_NumEntries; ++i)
		{
			writer.Int(m_HierarchyArray[i]);
		}

		writer.EndArray();

		writer.EndObject();
	}
}

namespace MR
{
	Manager::Manager()
	{
		nodeTypes[0] = new NodeDefControlParamFloat(20);
		nodeTypes[1] = new NodeDefControlParamVector3(21);
		nodeTypes[2] = new NodeDefControlParamVector4(22);
		nodeTypes[3] = new NodeDefOperatorFloat(110);
		nodeTypes[4] = new NodeDefOperator2Float(111);
		nodeTypes[5] = new NodeDefOperatorConst(112);
		nodeTypes[6] = new NodeDefOperatorNoiseGen(113);
		nodeTypes[7] = new NodeDefStateMachine(10);
		nodeTypes[8] = new NodeDefAnim(103);
		nodeTypes[9] = new NodeDefAnimWithEvents(104);
		nodeTypes[10] = new NodeDefBlend2(107);
		nodeTypes[11] = new NodeDefBlendN(108);
		nodeTypes[12] = new NodeDefBlend2MatchEvents(101);
		nodeTypes[13] = new NodeDefBlendNMatchEvents(102);
		nodeTypes[14] = new NodeDefFilterTransforms(105);
		nodeTypes[15] = new NodeDefSingleFrame(109);
		nodeTypes[16] = new NodeDefComputeWorldTransforms(106);
		nodeTypes[17] = new NodeDefFeatherBlend2(114);
		nodeTypes[18] = new NodeDefLockFoot(121);
		nodeTypes[19] = new NodeDefTwoBoneIK(120);
		nodeTypes[20] = new NodeDefHeadLook(122);
		nodeTypes[21] = new NodeDefApplyBindPose(115);
		nodeTypes[22] = new NodeDefTransit(402);
		nodeTypes[23] = new NodeDefTransitAtEvent(400);
		nodeTypes[24] = new NodeDefTransitMatchEvents(401);
		nodeTypes[25] = new NodeDefBlendTree(12);
		nodeTypes[26] = new ZNodeDefAnimIOI(65536);
		nodeTypes[27] = new ZNodeDefAnimIOICutSequence(65537);
		nodeTypes[28] = new ZNodeDefVariationIOI(65538);
		nodeTypes[29] = new ZNodeDefChildNetworkIOI(65539);
		nodeTypes[30] = new ZNodeDefSelectiveFeatherBlend(65540);
		nodeTypes[31] = new NodeDefBlend2ForwardEvents(65541);
		nodeTypes[32] = new MRSamples::NodeDefPlaySpeedModifier(125);
		nodeTypes[33] = new ZNodeDefGroundRotation(65542);
		nodeTypes[34] = new ZNodeDefScaleAnim(65545);
		nodeTypes[35] = new ZIOINodeLookAtDef(131072);
		nodeTypes[36] = new ZIOINodeSelectDef(131074);
		nodeTypes[37] = new ZNodeDefTransitionSelect(65544);
		nodeTypes[38] = new ZNodeDefIOITransit(131075);
		nodeTypes[39] = new NodeDefIOIDeltaAnim(131078);
		nodeTypes[40] = new NodeDefIOIDeltaAnimMatchEvents(131077);
		nodeTypes[41] = new NodeDefIOIHeadController(131079);
		nodeTypes[42] = new NodeDefIOIPelvisOffset(131080);
		nodeTypes[43] = new NodeDefIOIMirror(131086);
		nodeTypes[44] = new NodeDefIOIMirrorMatchEvents(131081);
		nodeTypes[45] = new NodeDefIOIBlend2AndReTarget(131082);
		nodeTypes[46] = new NodeDefPlaySpeedModifierMatchEvents(11);
		nodeTypes[47] = new NodeDefIOIPartialMirrorAim(131084);
		nodeTypes[48] = new NodeDefBlend2ForwardEvents(131085);
		nodeTypes[49] = new ZNodeDefIOISneakModifier(131087);
		nodeTypes[50] = new ZNodeDefIOITranslationScale(131088);
		nodeTypes[51] = new ZNodeDefIOIWildCardTransitMatchEvents(131089);
		nodeTypes[52] = new ZNodeDefIOIWildCardTransit(131090);
		nodeTypes[53] = new ZNodeDefSelectMatchEvents(65543);

		transitConditionTypes[0] = new TransitConditionDefFalse(607);
		transitConditionTypes[1] = new TransitConditionDefOnRequest(601);
		transitConditionTypes[2] = new TransitConditionDefInEventSegment(605);
		transitConditionTypes[3] = new TransitConditionDefCrossedEventBoundary(604);
		transitConditionTypes[4] = new TransitConditionDefCrossedDurationFraction(603);
		transitConditionTypes[5] = new TransitConditionDefControlParamFloatGreater(606);
		transitConditionTypes[6] = new TransitConditionDefControlParamFloatLess(608);
		transitConditionTypes[7] = new TransitConditionDefControlParamFloatInRange(609);
		transitConditionTypes[8] = new TransitConditionDefDiscreteEventTriggered(602);
		transitConditionTypes[9] = new TransitConditionDefInEventRange(611);
		transitConditionTypes[10] = new TransitConditionDefPercentageThroughSubtransition(131472);
	}

	Manager::~Manager()
	{
		delete[] & nodeTypes;
		delete[] & transitConditionTypes;
	}

	Manager& Manager::GetInstance()
	{
		static Manager instance;

		return instance;
	}

	void Manager::Construct(NodeDef* nodeDef)
	{
		for (size_t i = 0; i < 54; ++i)
		{
			if (nodeTypes[i]->GetTypeID() == nodeDef->GetTypeID())
			{
				void** vTable = *reinterpret_cast<void***>(nodeTypes[i]);
				*reinterpret_cast<void***>(nodeDef) = vTable;

				break;
			}
		}
	}

	void Manager::Construct(TransitConditionDef* transitConditionDef)
	{
		for (size_t i = 0; i < 11; ++i)
		{
			if (transitConditionTypes[i]->GetType() == transitConditionDef->GetType())
			{
				void** vTable = *reinterpret_cast<void***>(transitConditionTypes[i]);
				*reinterpret_cast<void***>(transitConditionDef) = vTable;

				break;
			}
		}
	}

	std::string Operator2Float::ConvertOperationTypeToString(OperationType operationType)
	{
		switch (operationType)
		{
			case OperationType::OPERATION_MULTIPLY:
				return "OPERATION_MULTIPLY";
			case OperationType::OPERATION_ADD:
				return "OPERATION_ADD";
			case OperationType::OPERATION_DIVIDE:
				return "OPERATION_DIVIDE";
			case OperationType::OPERATION_SUBTRACT:
				return "OPERATION_SUBTRACT";
			case OperationType::OPERATION_MIN:
				return "OPERATION_MIN";
			case OperationType::OPERATION_MAX:
				return "OPERATION_MAX";
			case OperationType::OPERATION_NUMOPS:
				return "OPERATION_NUMOPS";
			case OperationType::OPERATION_FORCEINTSIZE:
				return "OPERATION_FORCEINTSIZE";
			default:
				return "";
		}
	}

	TransitConditionDef::TransitConditionDef(unsigned int nTypeId)
	{
		m_type = nTypeId;
		m_offset = -1;
	}

	unsigned int TransitConditionDef::GetType() const
	{
		return m_type;
	}

	void TransitConditionDef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.String("m_type");
		writer.Uint(m_type);

		writer.String("m_offset");
		writer.Uint(m_offset);
	}

	TransitConditionDefFalse::TransitConditionDefFalse(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
	}

	void TransitConditionDefFalse::Locate()
	{

	}

	TransitConditionDefOnRequest::TransitConditionDefOnRequest(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
		m_requestID = 0;
		m_onNotSet = false;
	}

	void TransitConditionDefOnRequest::Locate()
	{

	}

	void TransitConditionDefOnRequest::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		TransitConditionDef::SerializeToJson(writer);

		writer.String("m_requestID");
		writer.Uint(m_requestID);

		writer.String("m_onNotSet");
		writer.Bool(m_onNotSet);

		writer.EndObject();
	}

	TransitConditionDefInEventSegment::TransitConditionDefInEventSegment(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
		m_eventSegmentIndex = 0;
	}

	void TransitConditionDefInEventSegment::Locate()
	{

	}

	void TransitConditionDefInEventSegment::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		TransitConditionDef::SerializeToJson(writer);

		writer.String("m_eventSegmentIndex");
		writer.Uint(m_eventSegmentIndex);

		writer.EndObject();
	}

	TransitConditionDefCrossedEventBoundary::TransitConditionDefCrossedEventBoundary(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
		m_eventTriggerPosition = 0;
	}

	void TransitConditionDefCrossedEventBoundary::Locate()
	{

	}

	void TransitConditionDefCrossedEventBoundary::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		TransitConditionDef::SerializeToJson(writer);

		writer.String("m_eventTriggerPosition");
		writer.Double(m_eventTriggerPosition);

		writer.EndObject();
	}

	TransitConditionDefCrossedDurationFraction::TransitConditionDefCrossedDurationFraction(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
		m_durationFractionTrigger = 0;
	}

	void TransitConditionDefCrossedDurationFraction::Locate()
	{

	}

	void TransitConditionDefCrossedDurationFraction::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		TransitConditionDef::SerializeToJson(writer);

		writer.String("m_durationFractionTrigger");
		writer.Double(m_durationFractionTrigger);

		writer.EndObject();
	}

	TransitConditionDefControlParamFloatGreater::TransitConditionDefControlParamFloatGreater(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
		m_controlParamNodeID = 0;
		m_testValue = 0;
		m_orEqual = false;
	}

	void TransitConditionDefControlParamFloatGreater::Locate()
	{

	}

	void TransitConditionDefControlParamFloatGreater::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		TransitConditionDef::SerializeToJson(writer);

		writer.String("m_controlParamNodeID");
		writer.Uint(m_controlParamNodeID);

		writer.String("m_testValue");
		writer.Double(m_testValue);

		writer.String("m_orEqual");
		writer.Double(m_orEqual);

		writer.EndObject();
	}

	TransitConditionDefControlParamFloatLess::TransitConditionDefControlParamFloatLess(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
		m_controlParamNodeID = 0;
		m_testValue = 0;
		m_orEqual = false;
	}

	void TransitConditionDefControlParamFloatLess::Locate()
	{

	}

	void TransitConditionDefControlParamFloatLess::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		TransitConditionDef::SerializeToJson(writer);

		writer.String("m_controlParamNodeID");
		writer.Uint(m_controlParamNodeID);

		writer.String("m_testValue");
		writer.Double(m_testValue);

		writer.String("m_orEqual");
		writer.Bool(m_orEqual);

		writer.EndObject();
	}

	TransitConditionDefControlParamFloatInRange::TransitConditionDefControlParamFloatInRange(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
		m_controlParamNodeID = 0;
		m_lowerTestValue = 0;
		m_upperTestValue = 0;
		m_notInRange = false;
	}

	void TransitConditionDefControlParamFloatInRange::Locate()
	{

	}

	void TransitConditionDefControlParamFloatInRange::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		TransitConditionDef::SerializeToJson(writer);

		writer.String("m_controlParamNodeID");
		writer.Uint(m_controlParamNodeID);

		writer.String("m_lowerTestValue");
		writer.Double(m_lowerTestValue);

		writer.String("m_upperTestValue");
		writer.Double(m_upperTestValue);

		writer.String("m_notInRange");
		writer.Double(m_notInRange);

		writer.EndObject();
	}

	TransitConditionDefDiscreteEventTriggered::TransitConditionDefDiscreteEventTriggered(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
		m_eventUserData = 0;
		m_eventWeightThreshold = 0;
		m_orEqual = false;
	}

	void TransitConditionDefDiscreteEventTriggered::Locate()
	{

	}

	void TransitConditionDefDiscreteEventTriggered::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		TransitConditionDef::SerializeToJson(writer);

		writer.String("m_eventUserData");
		writer.Uint(m_eventUserData);

		writer.String("m_eventWeightThreshold");
		writer.Double(m_eventWeightThreshold);

		writer.String("m_orEqual");
		writer.Bool(m_orEqual);

		writer.EndObject();
	}

	TransitConditionDefInEventRange::TransitConditionDefInEventRange(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
		m_eventRangeStart = 0;
		m_eventRangeEnd = 0;
	}

	void TransitConditionDefInEventRange::Locate()
	{

	}

	void TransitConditionDefInEventRange::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		TransitConditionDef::SerializeToJson(writer);

		writer.String("m_eventRangeStart");
		writer.Double(m_eventRangeStart);

		writer.String("m_eventRangeEnd");
		writer.Double(m_eventRangeEnd);

		writer.EndObject();
	}

	TransitConditionDefPercentageThroughSubtransition::TransitConditionDefPercentageThroughSubtransition(unsigned int nTypeId) : TransitConditionDef(nTypeId)
	{
		m_fPercentageThrough = 0;
		m_nSubStateID = 0;
		m_bUseTransitionSource = false;
		m_bLessThan = false;
	}

	void TransitConditionDefPercentageThroughSubtransition::Locate()
	{

	}

	void TransitConditionDefPercentageThroughSubtransition::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		TransitConditionDef::SerializeToJson(writer);

		writer.String("m_fPercentageThrough");
		writer.Double(m_fPercentageThrough);

		writer.String("m_nSubStateID");
		writer.Uint(m_nSubStateID);

		writer.String("m_bUseTransitionSource");
		writer.Bool(m_bUseTransitionSource);

		writer.String("m_bLessThan");
		writer.Bool(m_bLessThan);

		writer.EndObject();
	}

	NodeDef::NodeDef(unsigned int typeID) : m_typeID(typeID), m_nodeID(-1), m_pName(nullptr), m_pParent(nullptr)
	{
	}

	void NodeDef::Locate()
	{
		m_pName += reinterpret_cast<uintptr_t>(this);

		if (m_pParent)
		{
			m_pParent = reinterpret_cast<const NodeDef*>(reinterpret_cast<uintptr_t>(m_pParent) + reinterpret_cast<uintptr_t>(this));
		}
	}

	unsigned int NodeDef::GetTypeID() const
	{
		return m_typeID;
	}

	void NodeDef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.String("m_typeID");
		writer.Uint(m_typeID);

		writer.String("m_nodeID");
		writer.Uint(m_nodeID);

		writer.String("m_pName");
		writer.String(m_pName);

		writer.String("m_pParent");

		if (m_pParent)
		{
			//m_pParent->SerializeToJson(writer);
			writer.String(m_pParent->m_pName);
		}
		else
		{
			writer.Null();
		}
	}

	NodeDefControlParam::NodeDefControlParam(unsigned int typeID) : NodeDef(typeID)
	{
	}

	NodeDefControlParamFloat::NodeDefControlParamFloat(unsigned int typeID) : NodeDefControlParam(typeID)
	{
		m_defaultVal = 0;
	}

	void NodeDefControlParamFloat::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefControlParam::SerializeToJson(writer);

		writer.String("m_defaultVal");
		writer.Double(m_defaultVal);

		writer.EndObject();
	}

	NodeDefControlParamVector3::NodeDefControlParamVector3(unsigned int typeID) : NodeDefControlParam(typeID)
	{
	}

	void NodeDefControlParamVector3::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefControlParam::SerializeToJson(writer);

		writer.String("m_pad");
		writer.StartArray();

		for (unsigned int i = 0; i < 12; ++i)
		{
			writer.Uint(m_pad[i]);
		}

		writer.EndArray();

		writer.String("m_defaultVal");
		m_defaultVal.SerializeToJson(writer);

		writer.EndObject();
	}

	NodeDefControlParamVector4::NodeDefControlParamVector4(unsigned int typeID) : NodeDefControlParam(typeID)
	{
	}

	void NodeDefControlParamVector4::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefControlParam::SerializeToJson(writer);

		writer.String("m_pad");
		writer.StartArray();

		for (unsigned int i = 0; i < 12; ++i)
		{
			writer.Uint(m_pad[i]);
		}

		writer.EndArray();

		writer.String("m_defaultVal");
		m_defaultVal.SerializeToJson(writer);

		writer.EndObject();
	}

	NodeDefOperatorFloat::NodeDefOperatorFloat(unsigned int typeID) : NodeDefControlParam(typeID)
	{
	}

	void NodeDefOperatorFloat::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefControlParam::SerializeToJson(writer);

		writer.String("m_input");
		writer.Uint(m_input);

		writer.String("m_operation");
		writer.String(Operator2Float::ConvertOperationTypeToString(m_operation).c_str());

		writer.EndObject();
	}

	NodeDefOperator2Float::NodeDefOperator2Float(unsigned int typeID) : NodeDefControlParam(typeID)
	{
	}

	void NodeDefOperator2Float::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefControlParam::SerializeToJson(writer);

		writer.String("m_input1");
		writer.Uint(m_input1);

		writer.String("m_input2");
		writer.Uint(m_input2);

		writer.String("m_operation");
		writer.String(Operator2Float::ConvertOperationTypeToString(m_operation).c_str());

		writer.EndObject();
	}

	NodeDefOperatorConst::NodeDefOperatorConst(unsigned int typeID) : NodeDefControlParam(typeID)
	{
	}

	void NodeDefOperatorConst::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefControlParam::SerializeToJson(writer);

		writer.String("m_input");
		writer.Uint(m_input);

		writer.String("m_operation");
		writer.String(Operator2Float::ConvertOperationTypeToString(m_operation).c_str());

		writer.String("m_constValue");
		writer.Double(m_constValue);

		writer.EndObject();
	}

	NodeDefOperatorNoiseGen::NodeDefOperatorNoiseGen(unsigned int typeID) : NodeDefControlParam(typeID)
	{
	}

	void NodeDefOperatorNoiseGen::Locate()
	{
		NodeDef::Locate();

		m_table1 = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_table1) + reinterpret_cast<uintptr_t>(this));
		m_table2 = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_table2) + reinterpret_cast<uintptr_t>(this));
		m_table3 = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_table3) + reinterpret_cast<uintptr_t>(this));
	}

	void NodeDefOperatorNoiseGen::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefControlParam::SerializeToJson(writer);

		writer.String("m_input");
		writer.Uint(m_input);

		writer.String("m_frequency");
		writer.Double(m_frequency);

		writer.String("m_noiseFlags");
		writer.Uint(m_noiseFlags);

		writer.String("m_table1");
		writer.StartArray();

		for (unsigned int i = 0; i < 7; ++i)
		{
			writer.Double(m_table1[i]);
		}

		writer.EndArray();

		writer.String("m_table2");
		writer.StartArray();

		for (unsigned int i = 0; i < 19; ++i)
		{
			writer.Double(m_table2[i]);
		}

		writer.EndArray();

		writer.String("m_table3");
		writer.StartArray();

		for (unsigned int i = 0; i < 31; ++i)
		{
			writer.Double(m_table3[i]);
		}

		writer.EndArray();

		writer.EndObject();
	}

	NodeDefPassThrough::NodeDefPassThrough(unsigned int typeID) : NodeDef(typeID)
	{
	}

	void NodeDefPassThrough::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		NodeDef::SerializeToJson(writer);

		writer.String("m_nSourceNodeId");
		writer.Uint(m_nSourceNodeId);
	}

	void StateChangeDef::Locate()
	{
		m_conditionIndexes = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_conditionIndexes) + reinterpret_cast<uintptr_t>(this));
	}

	void StateChangeDef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		writer.String("m_destinationStateID");
		writer.Uint(m_destinationStateID);

		writer.String("m_numConditions");
		writer.Uint(m_numConditions);

		writer.String("m_conditionIndexes");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numConditions; ++i)
		{
			writer.Uint(m_conditionIndexes[i]);
		}

		writer.EndArray();

		writer.EndObject();
	}

	void StateDef::Locate()
	{
		m_conditionIndexes = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_conditionIndexes) + reinterpret_cast<uintptr_t>(this));
		m_stateChanges = reinterpret_cast<StateChangeDef*>(reinterpret_cast<uintptr_t>(m_stateChanges) + reinterpret_cast<uintptr_t>(this));

		for (unsigned int i = 0; i < m_numStateChanges; ++i)
		{
			m_stateChanges[i].Locate();
		}
	}

	void StateDef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		writer.String("m_nodeID");
		writer.Uint(m_nodeID);

		writer.String("m_numConditions");
		writer.Uint(m_numConditions);

		writer.String("m_conditionIndexes");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numConditions; ++i)
		{
			writer.Uint(m_conditionIndexes[i]);
		}

		writer.EndArray();

		writer.String("m_numStateChanges");
		writer.Uint(m_numStateChanges);

		writer.String("m_stateChanges");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numStateChanges; ++i)
		{
			m_stateChanges[i].SerializeToJson(writer);
		}

		writer.EndArray();

		writer.EndObject();
	}

	NodeDefStateMachine::NodeDefStateMachine(unsigned int typeID) : NodeDefPassThrough(typeID)
	{
	}

	void NodeDefStateMachine::Locate()
	{
		NodeDef::Locate();

		if (m_children)
		{
			m_children = reinterpret_cast<NodeDef**>(reinterpret_cast<uintptr_t>(m_children) + reinterpret_cast<uintptr_t>(this));
		}

		for (unsigned int i = 0; i < m_numChildren; ++i)
		{
			m_children[i] = reinterpret_cast<NodeDef*>(reinterpret_cast<uintptr_t>(m_children[i]) + reinterpret_cast<uintptr_t>(this));
		}

		m_conditions = reinterpret_cast<TransitConditionDef**>(reinterpret_cast<uintptr_t>(m_conditions) + reinterpret_cast<uintptr_t>(this));

		for (unsigned int i = 0; i < m_numConditions; ++i)
		{
			m_conditions[i] = reinterpret_cast<TransitConditionDef*>(reinterpret_cast<uintptr_t>(m_conditions[i]) + reinterpret_cast<uintptr_t>(this));

			Manager::GetInstance().Construct(m_conditions[i]);

			m_conditions[i]->Locate();
		}

		m_stateDefs = reinterpret_cast<StateDef*>(reinterpret_cast<uintptr_t>(m_stateDefs) + reinterpret_cast<uintptr_t>(this));

		for (unsigned int i = 0; i < m_numStates; ++i)
		{
			m_stateDefs[i].Locate();
		}
	}

	void NodeDefStateMachine::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefPassThrough::SerializeToJson(writer);

		writer.String("m_numStates");
		writer.Uint(m_numStates);

		writer.String("m_stateDefs");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numStates; ++i)
		{
			m_stateDefs[i].SerializeToJson(writer);
		}

		writer.EndArray();

		writer.String("m_numConditions");
		writer.Uint(m_numConditions);

		writer.String("m_conditions");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numConditions; ++i)
		{
			m_conditions[i]->SerializeToJson(writer);
		}

		writer.EndArray();

		writer.String("m_numChildren");
		writer.Uint(m_numChildren);

		writer.String("m_children");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numChildren; ++i)
		{
			m_children[i]->SerializeToJson(writer);
		}

		writer.EndArray();

		writer.EndObject();
	}

	NodeDefAnim::NodeDefAnim(unsigned int typeID) : NodeDef(typeID)
	{
	}

	void NodeDefAnim::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		if (startObject)
		{
			writer.StartObject();
		}

		NodeDef::SerializeToJson(writer);

		writer.String("m_generateDeltas");
		writer.Bool(m_generateDeltas);

		writer.String("m_loop");
		writer.Bool(m_loop);

		writer.String("m_playBackwards");
		writer.Bool(m_playBackwards);

		writer.String("m_clipStartFraction");
		writer.Double(m_clipStartFraction);

		writer.String("m_clipEndFraction");
		writer.Double(m_clipEndFraction);

		writer.String("m_animSource");
		writer.Null();

		if (startObject)
		{
			writer.EndObject();
		}
	}

	NodeDefAnimWithEvents::NodeDefAnimWithEvents(unsigned int typeID) : NodeDefAnim(typeID)
	{
	}

	void NodeDefAnimWithEvents::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		if (startObject)
		{
			writer.StartObject();
		}

		NodeDefAnim::SerializeToJson(writer, false);

		writer.String("m_startEventIndex");
		writer.Int(m_startEventIndex);

		if (startObject)
		{
			writer.EndObject();
		}
	}

	NodeDefBlendBase::NodeDefBlendBase(unsigned int typeID) : NodeDef(typeID)
	{
	}

	void NodeDefBlendBase::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		//writer.StartObject();

		NodeDef::SerializeToJson(writer);

		writer.String("m_aID");
		writer.Uint(m_aID);

		writer.String("m_bID");
		writer.Uint(m_bID);

		writer.String("m_UseSource1Additively");
		writer.Bool(m_UseSource1Additively);

		//writer.EndObject();
	}

	NodeDefBlend2::NodeDefBlend2(unsigned int typeID) : NodeDefBlendBase(typeID)
	{
	}

	void NodeDefBlend2::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		if (startObject)
		{
			writer.StartObject();
		}

		NodeDefBlendBase::SerializeToJson(writer);

		writer.String("m_weightNodeID");
		writer.Uint(m_weightNodeID);

		writer.String("m_loop");
		writer.Bool(m_loop);

		writer.String("m_additiveBlendAtt");
		writer.Bool(m_additiveBlendAtt);

		writer.String("m_additiveBlendPos");
		writer.Bool(m_additiveBlendPos);

		writer.String("m_slerpDeltaTrajTrans");
		writer.Bool(m_slerpDeltaTrajTrans);

		writer.String("m_source0BranchOptimization");
		writer.Bool(m_source0BranchOptimization);

		writer.String("m_source1BranchOptimization");
		writer.Bool(m_source1BranchOptimization);

		writer.String("m_ignoreSource0EventWeight");
		writer.Bool(m_ignoreSource0EventWeight);

		writer.String("m_ignoreSource1EventWeight");
		writer.Bool(m_ignoreSource1EventWeight);

		writer.String("m_bUpdateWeigthingOnce");
		writer.Bool(m_bUpdateWeigthingOnce);

		if (startObject)
		{
			writer.EndObject();
		}
	}

	NodeDefBlendN::NodeDefBlendN(unsigned int typeID) : NodeDefBlend2(typeID)
	{
	}

	void NodeDefBlendN::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefBlend2::SerializeToJson(writer, false);

		writer.String("m_numSourceNodes");
		writer.Uint(m_numSourceNodes);

		writer.String("m_connectedSourceNode");
		m_connectedSourceNode[0].SerializeToJson(writer);

		writer.EndObject();
	}

	void NodeDefBlendN::ConnectionDef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		writer.String("connectedNodeId");
		writer.Uint(connectedNodeId);

		writer.String("weight");
		writer.Double(weight);

		writer.EndObject();
	}

	NodeDefBlend2MatchEvents::NodeDefBlend2MatchEvents(unsigned int typeID) : NodeDefBlend2(typeID)
	{
	}

	void NodeDefBlend2MatchEvents::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		if (startObject)
		{
			writer.StartObject();
		}

		NodeDefBlend2::SerializeToJson(writer, false);

		writer.String("m_eventWeightNodeID");
		writer.Uint(m_eventWeightNodeID);

		writer.String("m_startEventIndex");
		writer.Int(m_startEventIndex);

		writer.String("m_durationEventMatchingType");
		writer.Uint(m_durationEventMatchingType);

		if (startObject)
		{
			writer.EndObject();
		}
	}

	NodeDefBlendNMatchEvents::NodeDefBlendNMatchEvents(unsigned int typeID) : NodeDefBlend2MatchEvents(typeID)
	{
	}

	void NodeDefBlendNMatchEvents::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefBlend2MatchEvents::SerializeToJson(writer, false);

		writer.String("m_numSourceNodes");
		writer.Uint(m_numSourceNodes);

		writer.String("m_connectedSourceNode");
		m_connectedSourceNode[0].SerializeToJson(writer);

		writer.EndObject();
	}

	void NodeDefBlendNMatchEvents::ConnectionDef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		writer.String("connectedNodeId");
		writer.Uint(connectedNodeId);

		writer.String("weight");
		writer.Double(weight);

		writer.EndObject();
	}

	NodeDefPassThroughTransforms::NodeDefPassThroughTransforms(unsigned int typeID) : NodeDefPassThrough(typeID)
	{
	}

	NodeDefFilterTransforms::NodeDefFilterTransforms(unsigned int typeID) : NodeDefPassThroughTransforms(typeID)
	{
	}

	void NodeDefFilterTransforms::Locate()
	{
		NodeDef::Locate();

		m_boneFilterIDs = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_boneFilterIDs) + reinterpret_cast<uintptr_t>(this));
	}

	void NodeDefFilterTransforms::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefPassThroughTransforms::SerializeToJson(writer);

		writer.String("m_numBoneFilterIDs");
		writer.Uint(m_numBoneFilterIDs);

		writer.String("m_boneFilterIDs");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numBoneFilterIDs; ++i)
		{
			writer.Uint(m_boneFilterIDs[i]);
		}

		writer.EndArray();

		writer.EndObject();
	}

	NodeDefSingleFrame::NodeDefSingleFrame(unsigned int typeID) : NodeDefPassThroughTransforms(typeID)
	{
	}

	void NodeDefSingleFrame::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefPassThroughTransforms::SerializeToJson(writer);

		writer.String("m_controlNodeID");
		writer.Uint(m_controlNodeID);

		writer.String("m_bUseTrajectory");
		writer.Bool(m_bUseTrajectory);

		writer.EndObject();
	}

	NodeDefComputeWorldTransforms::NodeDefComputeWorldTransforms(unsigned int typeID) : NodeDefPassThroughTransforms(typeID)
	{
	}

	void NodeRef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		writer.String("m_nodeId");
		writer.Uint(m_nodeId);

		writer.EndObject();
	}

	NodeDefFeatherBlend2::NodeDefFeatherBlend2(unsigned int typeID) : NodeDef(typeID)
	{
	}

	void NodeDefFeatherBlend2::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDef::SerializeToJson(writer);

		writer.String("m_aID");
		m_aID.SerializeToJson(writer);

		writer.String("m_bID");
		m_bID.SerializeToJson(writer);

		writer.String("m_weightNodeID");
		m_weightNodeID.SerializeToJson(writer);

		writer.String("m_numBoneWeights");
		writer.Uint(m_numBoneWeights);

		writer.String("m_boneWeights");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numBoneWeights; ++i)
		{
			writer.Double(m_boneWeights[i]);
		}

		writer.EndArray();

		writer.String("m_loop");
		writer.Bool(m_loop);

		writer.String("m_additiveBlendAtt");
		writer.Bool(m_additiveBlendAtt);

		writer.String("m_additiveBlendPos");
		writer.Bool(m_additiveBlendPos);

		writer.String("m_slerpDeltaTrajTrans");
		writer.Bool(m_slerpDeltaTrajTrans);

		writer.EndObject();
	}

	void NodeDefFeatherBlend2::Locate()
	{
		NodeDef::Locate();

		m_boneWeights = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_boneWeights) + reinterpret_cast<uintptr_t>(this));
	}

	void BoneId::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		writer.String("m_boneId");
		writer.Uint(m_boneId);

		writer.EndObject();
	}

	NodeDefLockFoot::NodeDefLockFoot(unsigned int typeID) : NodeDefPassThroughTransforms(typeID)
	{
	}

	void NodeDefLockFoot::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefPassThroughTransforms::SerializeToJson(writer);

		writer.String("m_catchupSpeedFactor");
		writer.Double(m_catchupSpeedFactor);

		writer.String("m_snapToSourceDistance");
		writer.Double(m_snapToSourceDistance);

		writer.String("m_straightestLegFactor");
		writer.Double(m_straightestLegFactor);

		writer.String("m_footfallEventID");
		writer.Int(m_footfallEventID);

		writer.String("m_lowerHeightBound");
		writer.Double(m_lowerHeightBound);

		writer.String("m_toeLowerHeightBound");
		writer.Double(m_toeLowerHeightBound);

		writer.String("m_kneeRotationAxis");
		m_kneeRotationAxis.SerializeToJson(writer);

		writer.String("m_ballRotationAxis");
		m_ballRotationAxis.SerializeToJson(writer);

		writer.String("m_upAxis");
		m_upAxis.SerializeToJson(writer);

		writer.String("m_IkFkBlendWeightNodeID");
		m_IkFkBlendWeightNodeID.SerializeToJson(writer);

		writer.String("m_swivelContributionToOrientationNodeID");
		m_swivelContributionToOrientationNodeID.SerializeToJson(writer);

		writer.String("m_pad");
		writer.Uint(m_pad);

		writer.String("m_hipIndex");
		m_hipIndex.SerializeToJson(writer);

		writer.String("m_kneeIndex");
		m_kneeIndex.SerializeToJson(writer);

		writer.String("m_ankleIndex");
		m_ankleIndex.SerializeToJson(writer);

		writer.String("m_ballIndex");
		m_ballIndex.SerializeToJson(writer);

		writer.String("m_toeIndex");
		m_toeIndex.SerializeToJson(writer);

		writer.String("m_fixGroundPenetration");
		writer.Bool(m_fixGroundPenetration);

		writer.String("m_flipKneeRotationDirection");
		writer.Bool(m_flipKneeRotationDirection);

		writer.String("m_useBallJoint");
		writer.Bool(m_useBallJoint);

		writer.String("m_assumeSimpleHierarchy");
		writer.Bool(m_assumeSimpleHierarchy);

		writer.String("m_fixToeGroundPenetration");
		writer.Bool(m_fixToeGroundPenetration);

		writer.String("m_valid");
		writer.Bool(m_valid);

		writer.EndObject();
	}

	NodeDefTwoBoneIK::NodeDefTwoBoneIK(unsigned int typeID) : NodeDefPassThroughTransforms(typeID)
	{
	}

	void NodeDefTwoBoneIK::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefPassThroughTransforms::SerializeToJson(writer);

		writer.String("m_endJointIndex");
		m_endJointIndex.SerializeToJson(writer);

		writer.String("m_midJointIndex");
		m_midJointIndex.SerializeToJson(writer);

		writer.String("m_rootJointIndex");
		m_rootJointIndex.SerializeToJson(writer);

		writer.String("m_referenceJointIndex");
		m_referenceJointIndex.SerializeToJson(writer);

		writer.String("m_effectorTargetNodeID");
		m_effectorTargetNodeID.SerializeToJson(writer);

		writer.String("m_targetOrientationNodeID");
		m_targetOrientationNodeID.SerializeToJson(writer);

		writer.String("m_swivelAngleNodeID");
		m_swivelAngleNodeID.SerializeToJson(writer);

		writer.String("m_IkFkBlendWeightNodeID");
		m_IkFkBlendWeightNodeID.SerializeToJson(writer);

		writer.String("m_swivelContributionToOrientationNodeID");
		m_swivelContributionToOrientationNodeID.SerializeToJson(writer);

		writer.String("m_midJointRotationAxisX");
		writer.Double(m_midJointRotationAxisX);

		writer.String("m_midJointRotationAxisY");
		writer.Double(m_midJointRotationAxisY);

		writer.String("m_midJointRotationAxisZ");
		writer.Double(m_midJointRotationAxisZ);

		writer.String("m_flipMidJointRotationDirection");
		writer.Bool(m_flipMidJointRotationDirection);

		writer.String("m_assumeSimpleHierarchy");
		writer.Bool(m_assumeSimpleHierarchy);

		writer.String("m_keepEndEffOrientation");
		writer.Bool(m_keepEndEffOrientation);

		writer.String("m_endEffOrientationFromReferenceJoint");
		writer.Bool(m_endEffOrientationFromReferenceJoint);

		writer.String("m_globalReferenceAxis");
		writer.Bool(m_globalReferenceAxis);

		writer.String("m_updateTargetByDeltas");
		writer.Bool(m_updateTargetByDeltas);

		writer.String("m_midJointReferenceAxisX");
		writer.Double(m_midJointReferenceAxisX);

		writer.String("m_midJointReferenceAxisY");
		writer.Double(m_midJointReferenceAxisY);

		writer.String("m_midJointReferenceAxisZ");
		writer.Double(m_midJointReferenceAxisZ);

		writer.EndObject();
	}

	NodeDefHeadLook::NodeDefHeadLook(unsigned int typeID) : NodeDefPassThroughTransforms(typeID)
	{
	}

	void NodeDefHeadLook::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefPassThroughTransforms::SerializeToJson(writer);

		writer.String("m_endJointIndex");
		m_endJointIndex.SerializeToJson(writer);

		writer.String("m_rootJointIndex");
		m_rootJointIndex.SerializeToJson(writer);

		writer.String("m_pointingJointIndex");
		m_pointingJointIndex.SerializeToJson(writer);

		writer.String("m_targetNodeID");
		m_targetNodeID.SerializeToJson(writer);

		writer.String("m_pointingJointAxisNodeID");
		m_pointingJointAxisNodeID.SerializeToJson(writer);

		writer.String("m_pointingJointIDNodeID");
		m_pointingJointIDNodeID.SerializeToJson(writer);

		writer.String("m_blendWeightNodeID");
		m_blendWeightNodeID.SerializeToJson(writer);

		writer.String("m_pointingJointAxisX");
		writer.Double(m_pointingJointAxisX);

		writer.String("m_pointingJointAxisY");
		writer.Double(m_pointingJointAxisY);

		writer.String("m_pointingJointAxisZ");
		writer.Double(m_pointingJointAxisZ);

		writer.String("m_bias");
		writer.Bool(m_bias);

		writer.String("m_updateTargetByDeltas");
		writer.Bool(m_updateTargetByDeltas);

		writer.String("m_pointingVectorInCS");
		writer.Bool(m_pointingVectorInCS);

		writer.EndObject();
	}

	NodeDefApplyBindPose::NodeDefApplyBindPose(unsigned int typeID) : NodeDefPassThroughTransforms(typeID)
	{
	}

	NodeDefTransit::NodeDefTransit(unsigned int typeID) : NodeDefBlendBase(typeID)
	{
	}

	void NodeDefTransit::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		if (startObject)
		{
			writer.StartObject();
		}

		NodeDefBlendBase::SerializeToJson(writer);

		writer.String("m_transitionDuration");
		writer.Double(m_transitionDuration);

		writer.String("m_bIgnoreSourceTrajectory");
		writer.Bool(m_bIgnoreSourceTrajectory);

		writer.String("m_bEnableRealign");
		writer.Bool(m_bEnableRealign);

		if (startObject)
		{
			writer.EndObject();
		}
	}

	NodeDefTransitAtEvent::NodeDefTransitAtEvent(unsigned int typeID) : NodeDefTransit(typeID)
	{
	}

	void NodeDefTransitAtEvent::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefTransit::SerializeToJson(writer, false);

		writer.String("m_destStartEventIndex");
		writer.Uint(m_destStartEventIndex);

		writer.String("m_startFromSetFractionOfDestEvent");
		writer.Bool(m_startFromSetFractionOfDestEvent);

		writer.String("m_destStartEventFraction");
		writer.Double(m_destStartEventFraction);

		writer.EndObject();
	}

	NodeDefTransitMatchEvents::NodeDefTransitMatchEvents(unsigned int typeID) : NodeDefTransit(typeID)
	{
	}

	void NodeDefTransitMatchEvents::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDefTransit::SerializeToJson(writer, false);

		writer.String("m_destEventSequenceOffset");
		writer.Uint(m_destEventSequenceOffset);

		writer.String("m_destStartEventIndex");
		writer.Uint(m_destStartEventIndex);

		writer.String("m_usingDestStartEventIndex");
		writer.Bool(m_usingDestStartEventIndex);

		writer.String("m_durationEventMatchingType");
		writer.Uint(m_durationEventMatchingType);

		writer.EndObject();
	}

	NodeDefBlendTree::NodeDefBlendTree(unsigned int typeID) : NodeDef(typeID)
	{
	}

	void NodeDefBlendTree::Locate()
	{
		NodeDef::Locate();

		m_children = reinterpret_cast<NodeDef**>(reinterpret_cast<uintptr_t>(m_children) + reinterpret_cast<uintptr_t>(this));

		for (unsigned int i = 0; i < m_nNumChildren; ++i)
		{
			m_children[i] = reinterpret_cast<NodeDef*>(reinterpret_cast<uintptr_t>(m_children[i]) + reinterpret_cast<uintptr_t>(this));
		}
	}

	void NodeDefBlendTree::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
	{
		writer.StartObject();

		NodeDef::SerializeToJson(writer);

		writer.String("m_nNumChildren");
		writer.Uint(m_nNumChildren);

		writer.String("m_children");
		writer.StartArray();

		for (unsigned int i = 0; i < m_nNumChildren; ++i)
		{
			m_children[i]->SerializeToJson(writer);
		}

		writer.EndArray();

		writer.String("m_nRootNodeId");
		writer.Uint(m_nRootNodeId);

		writer.EndObject();
	}

	void TransformBuffersCount::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject)
	{
		writer.StartObject();

		writer.String("m_numTranformBufferTypesUsed");
		writer.Uint(m_numTranformBufferTypesUsed);

		writer.String("m_transformBufferTypes");
		writer.StartArray();

		for (unsigned int i = 0; i < 8; ++i)
		{
			writer.Uint(m_transformBufferTypes[i]);
		}

		writer.EndArray();

		writer.String("m_transformBufferCounts");
		writer.StartArray();

		for (unsigned int i = 0; i < 8; ++i)
		{
			writer.Uint(m_transformBufferCounts[i]);
		}

		writer.EndArray();

		writer.EndObject();
	}

	void NetworkDef::Locate()
	{
		m_nodeDefs = reinterpret_cast<NodeDef**>(reinterpret_cast<uintptr_t>(m_nodeDefs) + reinterpret_cast<uintptr_t>(this));
		m_persistentNodeIDs = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_persistentNodeIDs) + reinterpret_cast<uintptr_t>(this));

		if (m_numNodeStateMachines)
		{
			m_nodeStateMachineNodeIDs = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_nodeStateMachineNodeIDs) + reinterpret_cast<uintptr_t>(this));
		}

		if (m_numInterfaceControlParameters)
		{
			m_interfaceControlParameterNodeIDs = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_interfaceControlParameterNodeIDs) + reinterpret_cast<uintptr_t>(this));
		}

		if (m_requestIDStringTable)
		{
			m_requestIDStringTable = reinterpret_cast<NMP::StringTable*>(reinterpret_cast<uintptr_t>(m_requestIDStringTable) + reinterpret_cast<uintptr_t>(this));

			m_requestIDStringTable->Locate();
		}

		for (unsigned int i = 0; i < m_numNodeDefs + m_numDummyBlendTreeDefs; ++i)
		{
			m_nodeDefs[i] = reinterpret_cast<NodeDef*>(reinterpret_cast<uintptr_t>(m_nodeDefs[i]) + reinterpret_cast<uintptr_t>(this));

			Manager::GetInstance().Construct(m_nodeDefs[i]);

			m_nodeDefs[i]->Locate();
		}

		m_nodeNames += reinterpret_cast<uintptr_t>(this);
	}

	std::string NetworkDef::SerializeToJson()
	{
		rapidjson::StringBuffer stringBuffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

		writer.StartObject();

		writer.String("m_numAnimations");
		writer.Uint(m_numAnimations);

		writer.String("m_animLibrary");
		writer.Null();

		writer.String("m_rig");
		writer.Null();

		writer.String("m_numNodeNames");
		writer.Uint(m_numNodeNames);

		writer.String("m_nodeNames");
		writer.StartArray();

		unsigned int length = 0;

		for (unsigned int i = 0; i < m_numNodeNames; ++i)
		{
			const char* nodeName = m_nodeNames + length;

			writer.String(nodeName);

			length += strlen(nodeName) + 1;
		}

		writer.EndArray();

		writer.String("m_numNodeDefs");
		writer.Uint(m_numNodeDefs);

		writer.String("m_numDummyBlendTreeDefs");
		writer.Uint(m_numDummyBlendTreeDefs);

		writer.String("m_nodeDefs");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numNodeDefs + m_numDummyBlendTreeDefs; ++i)
		{
			m_nodeDefs[i]->SerializeToJson(writer);
		}

		writer.EndArray();

		writer.String("m_maxDurationEventsPoolSize");
		writer.Uint(m_maxDurationEventsPoolSize);

		writer.String("m_rootNodeID");
		writer.Uint(m_rootNodeID);

		writer.String("m_numNodeStateMachines");
		writer.Uint(m_numNodeStateMachines);

		writer.String("m_nodeStateMachineNodeIDs");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numNodeStateMachines; ++i)
		{
			writer.Uint(m_nodeStateMachineNodeIDs[i]);
		}

		writer.EndArray();

		writer.String("m_numInterfaceControlParameters");
		writer.Uint(m_numInterfaceControlParameters);

		writer.String("m_interfaceControlParameterNodeIDs");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numInterfaceControlParameters; ++i)
		{
			writer.Uint(m_interfaceControlParameterNodeIDs[i]);
		}

		writer.EndArray();

		writer.String("m_numPersistentNodes");
		writer.Uint(m_numPersistentNodes);

		writer.String("m_persistentNodeIDs");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numPersistentNodes; ++i)
		{
			writer.Uint(m_persistentNodeIDs[i]);
		}

		writer.EndArray();

		writer.String("m_networkHash");

		std::stringstream stringstream;

		stringstream << "0x" << std::hex << std::uppercase;

		for (unsigned int i = 0; i < 16; ++i)
		{
			stringstream << static_cast<int>(m_networkHash.b[i]);
		}

		writer.String(stringstream.str().c_str());

		writer.String("m_requestIDStringTable");
		m_requestIDStringTable->SerializeToJson(writer);

		writer.String("m_maxTransformBufferReqs");
		m_maxTransformBufferReqs.SerializeToJson(writer);

		writer.EndObject();

		return stringBuffer.GetString();
	}

	void Rig::Locate()
	{
		if (m_hierarchy)
		{
			m_hierarchy = reinterpret_cast<NMP::Hierarchy*>(reinterpret_cast<uintptr_t>(m_hierarchy) + reinterpret_cast<uintptr_t>(this));

			m_hierarchy->Locate();
		}

		if (m_boneNameMap)
		{
			m_boneNameMap = reinterpret_cast<NMP::StringTable*>(reinterpret_cast<uintptr_t>(m_boneNameMap) + reinterpret_cast<uintptr_t>(this));

			m_boneNameMap->Locate();
		}

		if (m_boneQuat)
		{
			m_boneQuat = reinterpret_cast<NMP::Quat*>(reinterpret_cast<uintptr_t>(m_boneQuat) + reinterpret_cast<uintptr_t>(this));
		}

		if (m_bonePos)
		{
			m_bonePos = reinterpret_cast<NMP::Vector3*>(reinterpret_cast<uintptr_t>(m_bonePos) + reinterpret_cast<uintptr_t>(this));
		}

		if (m_aGlobalIDToRigID)
		{
			m_aGlobalIDToRigID = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_aGlobalIDToRigID) + reinterpret_cast<uintptr_t>(this));
		}
	}

	std::string Rig::SerializeToJson()
	{
		rapidjson::StringBuffer stringBuffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

		writer.StartObject();

		writer.String("m_blendFrameOrientation");
		m_blendFrameOrientation.SerializeToJson(writer);

		writer.String("m_hierarchy");

		if (m_hierarchy)
		{
			m_hierarchy->SerializeToJson(writer);
		}
		else
		{
			writer.Null();
		}

		writer.String("m_trajectoryBoneIndex");
		writer.Uint(m_trajectoryBoneIndex);

		writer.String("m_characterRootBoneIndex");
		writer.Uint(m_characterRootBoneIndex);

		writer.String("m_boneNameMap");
		
		if (m_boneNameMap)
		{
			m_boneNameMap->SerializeToJson(writer);
		}
		else
		{
			writer.Null();
		}

		writer.String("m_boneQuat");

		if (m_boneQuat)
		{
			m_boneQuat->SerializeToJson(writer);
		}
		else
		{
			writer.Null();
		}

		writer.String("m_bonePos");

		if (m_bonePos)
		{
			m_bonePos->SerializeToJson(writer);
		}
		else
		{
			writer.Null();
		}

		writer.String("m_spuMemoryRequirements");
		writer.Uint(m_spuMemoryRequirements);

		writer.String("m_aGlobalIDToRigID");

		if (m_aGlobalIDToRigID)
		{
			writer.StartArray();

			for (unsigned int i = 0; i < m_nGlobalIDToRigIDCount; ++i)
			{
				writer.Uint(m_aGlobalIDToRigID[i]);
			}

			writer.EndArray();
		}
		else
		{
			writer.Null();
		}

		writer.EndObject();

		return stringBuffer.GetString();
	}

	void EventsSequenceDefBase::Locate()
	{
		if (m_name)
		{
			m_name = reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(m_name) + reinterpret_cast<uintptr_t>(this));
		}
	}

	void TimeFraction::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
	{
		writer.StartObject();

		writer.String("m_value");
		writer.Double(m_value);

		writer.EndObject();
	}

	void EventDef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, ECustomDataType customDataType, unsigned int* data)
	{
		writer.StartObject();

		writer.String("m_startTime");
		m_startTime.SerializeToJson(writer);

		writer.String("m_duration");
		m_duration.SerializeToJson(writer);

		writer.String("m_userData");
		writer.StartObject();

		writer.String("customDataType");
		writer.String(ConvertCustomDataTypeToString(customDataType).c_str());

		writer.String("data");

		switch (customDataType)
		{
			case ECustomDataType::eINT32:
				writer.Int(*data);
				break;
			case ECustomDataType::eFLOAT32:
				writer.Double(*data);
				break;
			case ECustomDataType::eCOLORRGB:
				writer.Uint(*data);
				break;
			case ECustomDataType::eRESOURCE:
				writer.Uint(*data);
				break;
			case ECustomDataType::eENUM:
				writer.Int(*data);
				break;
			case ECustomDataType::eSTRING:
			{
				char* string = reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(data) + *data);

				writer.String(string);

				break;
			}
		}

		writer.EndObject();

		writer.EndObject();
	}

	std::string EventDef::ConvertCustomDataTypeToString(const ECustomDataType customDataType)
	{
		switch (customDataType)
		{
			case ECustomDataType::eINT32:
				return "Int32";
			case ECustomDataType::eFLOAT32:
				return "Float32";
			case ECustomDataType::eCOLORRGB:
				return "ColorRGB";
			case ECustomDataType::eRESOURCE:
				return "Resource";
			case ECustomDataType::eENUM:
				return "Enum";
			case ECustomDataType::eSTRING:
				return "String";
			default:
				return "";
		}
	}

	void EventsSequenceDef::Locate()
	{
		MR::EventsSequenceDefBase::Locate();

		if (m_events)
		{
			m_events = reinterpret_cast<MR::EventDef*>(reinterpret_cast<uintptr_t>(m_events) + reinterpret_cast<uintptr_t>(this));
		}
	}

	void EventsSequenceDef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, unsigned int* customData)
	{
		writer.StartObject();

		writer.String("m_numEvents");
		writer.Uint(m_numEvents);

		writer.String("m_type");
		writer.Uint(m_type);

		writer.String("m_name");
		writer.String(m_name);

		writer.String("m_userData");
		writer.Uint(m_userData);

		writer.String("m_events");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numEvents; ++i)
		{
			ECustomDataType customDataType = static_cast<ECustomDataType>(customData[i + 1]);
			unsigned int customTypeCount = *customData;
			unsigned int* data = &customData[i + customTypeCount + 1];

			m_events[i].SerializeToJson(writer, customDataType, data);
		}

		writer.EndArray();

		writer.EndObject();
	}

	void EventDefDuration::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
	{
		writer.StartObject();

		writer.String("m_startTime");
		m_startTime.SerializeToJson(writer);

		writer.String("m_duration");
		m_duration.SerializeToJson(writer);

		writer.String("m_userData");
		writer.Uint(m_userData);

		writer.EndObject();
	}

	void EventsSequenceDefDuration::Locate()
	{
		MR::EventsSequenceDefBase::Locate();

		if (m_events)
		{
			m_events = reinterpret_cast<MR::EventDefDuration*>(reinterpret_cast<uintptr_t>(m_events) + reinterpret_cast<uintptr_t>(this));
		}
	}

	void EventsSequenceDefDuration::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, unsigned int* customData)
	{
		writer.StartObject();

		writer.String("m_events");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numEvents; ++i)
		{
			m_events[i].SerializeToJson(writer);
		}

		writer.EndArray();

		writer.EndObject();
	}

	void EventSequenceHeader::Locate()
	{
		if (m_eventSequences)
		{
			m_eventSequences = reinterpret_cast<MR::EventsSequenceDef**>(reinterpret_cast<uintptr_t>(m_eventSequences) + reinterpret_cast<uintptr_t>(this));

			for (unsigned int i = 0; i < m_numEventSequences; ++i)
			{
				m_eventSequences[i] = reinterpret_cast<EventsSequenceDef*>(reinterpret_cast<uintptr_t>(m_eventSequences[i]) + reinterpret_cast<uintptr_t>(this));

				m_eventSequences[i]->Locate();
			}
		}

		if (m_customData)
		{
			m_customData = reinterpret_cast<unsigned int**>(reinterpret_cast<uintptr_t>(m_customData) + reinterpret_cast<uintptr_t>(this));

			for (unsigned int i = 0; i < m_numEventSequences; ++i)
			{
				m_customData[i] = reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(m_customData[i]) + reinterpret_cast<uintptr_t>(this));
			}
		}

		if (m_durationEventSequences)
		{
			m_durationEventSequences = reinterpret_cast<MR::EventsSequenceDefDuration**>(reinterpret_cast<uintptr_t>(m_durationEventSequences) + reinterpret_cast<uintptr_t>(this));

			for (unsigned int i = 0; i < m_numDurationEventSequences; ++i)
			{
				m_durationEventSequences[i] = reinterpret_cast<EventsSequenceDefDuration*>(reinterpret_cast<uintptr_t>(m_durationEventSequences[i]) + reinterpret_cast<uintptr_t>(this));

				m_durationEventSequences[i]->Locate();
			}
		}
	}

	std::string EventSequenceHeader::SerializeToJson()
	{
		rapidjson::StringBuffer stringBuffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

		writer.StartObject();

		writer.String("m_timeStretchEventSequenceIndex");
		writer.Uint(m_timeStretchEventSequenceIndex);

		writer.String("m_numEventSequences");
		writer.Uint(m_numEventSequences);

		writer.String("m_eventSequences");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numEventSequences; ++i)
		{
			m_eventSequences[i]->SerializeToJson(writer, m_customData[i]);
		}

		writer.EndArray();

		writer.String("m_customData");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numEventSequences; ++i)
		{
			unsigned int* customData = m_customData[i];
			unsigned int customTypeCount = *customData;

			writer.StartObject();

			writer.String("customTypeCount");
			writer.Uint(customTypeCount);

			writer.String("customTypes");
			writer.StartArray();

			for (unsigned int j = 0; j < customTypeCount; ++j)
			{
				writer.StartObject();

				ECustomDataType customDataType = static_cast<ECustomDataType>(customData[j + 1]);
				unsigned int* data = &customData[j + customTypeCount + 1];

				writer.String("customDataType");
				writer.String(ConvertCustomDataTypeToString(customDataType).c_str());

				writer.String("data");

				switch (customDataType)
				{
					case ECustomDataType::eINT32:
						writer.Int(*data);
						break;
					case ECustomDataType::eFLOAT32:
						writer.Double(*data);
						break;
					case ECustomDataType::eCOLORRGB:
						writer.Uint(*data);
						break;
					case ECustomDataType::eRESOURCE:
						writer.Uint(*data);
						break;
					case ECustomDataType::eENUM:
						writer.Int(*data);
						break;
					case ECustomDataType::eSTRING:
					{
						char* string = reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(data) + *data);

						writer.String(string);

						break;
					}
				}

				writer.EndObject();
			}

			writer.EndArray();

			writer.EndObject();
		}

		writer.EndArray();

		writer.String("m_numDurationEventSequences");
		writer.Uint(m_numDurationEventSequences);

		writer.String("m_nodeStateMachineNodeIDs");
		writer.StartArray();

		for (unsigned int i = 0; i < m_numDurationEventSequences; ++i)
		{
			m_durationEventSequences[i]->SerializeToJson(writer, m_customData[i]);
		}

		writer.EndArray();

		writer.EndObject();

		return stringBuffer.GetString();
	}

	std::string EventSequenceHeader::ConvertCustomDataTypeToString(const ECustomDataType customDataType)
	{
		switch (customDataType)
		{
			case ECustomDataType::eINT32:
				return "Int32";
			case ECustomDataType::eFLOAT32:
				return "Float32";
			case ECustomDataType::eCOLORRGB:
				return "ColorRGB";
			case ECustomDataType::eRESOURCE:
				return "Resource";
			case ECustomDataType::eENUM:
				return "Enum";
			case ECustomDataType::eSTRING:
				return "String";
			default:
				return "";
		}
	}
}

ZNodeDefAnimIOI::ZNodeDefAnimIOI(unsigned int typeID) : NodeDefAnimWithEvents(typeID)
{
}

ZNodeDefAnimIOICutSequence::ZNodeDefAnimIOICutSequence(unsigned int typeID) : MR::NodeDef(typeID)
{
}

void ZNodeDefAnimIOICutSequence::Locate()
{
	MR::NodeDef::Locate();

	m_pszSlotName += reinterpret_cast<uintptr_t>(this);
}

void ZNodeDefAnimIOICutSequence::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDef::SerializeToJson(writer);

	writer.String("m_pszSlotName");
	writer.String(m_pszSlotName);

	writer.EndObject();
}

ZNodeDefVariationIOI::ZNodeDefVariationIOI(unsigned int typeID) : ZNodeDefAnimIOI(typeID)
{
}

void ZNodeDefVariationIOI::Locate()
{
	MR::NodeDefAnimWithEvents::Locate();

	m_pszVariationName += reinterpret_cast<uintptr_t>(this);
}

void ZNodeDefVariationIOI::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	ZNodeDefAnimIOI::SerializeToJson(writer, false);

	writer.String("m_pszVariationName");
	writer.String(m_pszVariationName);

	writer.String("m_bBlendFromLastFrame");
	writer.Bool(m_bBlendFromLastFrame);

	writer.EndObject();
}

ZNodeDefChildNetworkIOI::ZNodeDefChildNetworkIOI(unsigned int typeID) : MR::NodeDef(typeID)
{
}

ZNodeDefSelectiveFeatherBlend::ZNodeDefSelectiveFeatherBlend(unsigned int typeID) : MR::NodeDefBlend2(typeID)
{
}

void ZNodeDefSelectiveFeatherBlend::Locate()
{
	MR::NodeDefBlend2::Locate();

	m_paBoneWeights = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_paBoneWeights) + reinterpret_cast<uintptr_t>(this));
}

void ZNodeDefSelectiveFeatherBlend::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefBlend2::SerializeToJson(writer);

	writer.String("m_featherWeightNodeID");
	writer.Uint(m_featherWeightNodeID);

	writer.String("m_numBoneWeights");
	writer.Uint(m_numBoneWeights);

	writer.String("m_paBoneWeights");
	writer.StartArray();

	for (unsigned int i = 0; i < m_numBoneWeights; ++i)
	{
		writer.Double(m_paBoneWeights[i]);
	}

	writer.EndArray();

	writer.EndObject();
}

ZNodeDefGroundRotation::ZNodeDefGroundRotation(unsigned int typeID) : MR::NodeDefPassThrough(typeID)
{
}

void ZNodeDefGroundRotation::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThrough::SerializeToJson(writer);

	writer.String("m_nAngleNodeId");
	writer.Uint(m_nAngleNodeId);

	writer.String("m_nTrajectoryNodeID");
	writer.Uint(m_nTrajectoryNodeID);

	writer.String("m_nTranslationThresholdNodeID");
	writer.Uint(m_nTranslationThresholdNodeID);

	writer.String("m_nAttitudeThresholdNodeID");
	writer.Uint(m_nAttitudeThresholdNodeID);

	writer.EndObject();
}

ZNodeDefScaleAnim::ZNodeDefScaleAnim(unsigned int typeID) : MR::NodeDefPassThroughTransforms(typeID)
{
}

void ZNodeDefScaleAnim::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThroughTransforms::SerializeToJson(writer);

	writer.String("m_nBoneScaleNodeId");
	writer.Uint(m_nBoneScaleNodeId);

	writer.String("m_nTrajectoryTranslationScaleNodeID");
	writer.Uint(m_nTrajectoryTranslationScaleNodeID);

	writer.String("m_nTrajectoryOrientationScaleNodeID");
	writer.Uint(m_nTrajectoryOrientationScaleNodeID);

	writer.String("m_nTimeScaleNodeId");
	writer.Uint(m_nTimeScaleNodeId);

	writer.String("m_nReferenceNodeId");
	m_nReferenceNodeId.SerializeToJson(writer);

	writer.EndObject();
}

void SNodeLookAtIDs::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	writer.String("m_directionNodeID");
	writer.Uint(m_directionNodeID);

	writer.String("m_weightNodeID");
	writer.Uint(m_weightNodeID);

	writer.String("m_iHeadBoneIndex");
	writer.Uint(m_iHeadBoneIndex);

	writer.EndObject();
}

ZIOINodeLookAtDef::ZIOINodeLookAtDef(unsigned int typeID) : MR::NodeDefPassThroughTransforms(typeID)
{
}

void ZIOINodeLookAtDef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThroughTransforms::SerializeToJson(writer);

	writer.String("m_NodeIDs");
	m_NodeIDs.SerializeToJson(writer);

	writer.EndObject();
}

void SNodeSelectIDs::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	writer.String("m_sourceNodeIDs");
	writer.StartArray();

	for (unsigned int i = 0; i < 24; ++i)
	{
		writer.Uint(m_sourceNodeIDs[i]);
	}

	writer.EndArray();

	writer.String("m_selectCtrlID");
	writer.Uint(m_selectCtrlID);

	writer.String("m_blendCtrlID");
	writer.Uint(m_blendCtrlID);

	writer.EndObject();
}

ZIOINodeSelectDef::ZIOINodeSelectDef(unsigned int typeID) : MR::NodeDef(typeID)
{
}

void ZIOINodeSelectDef::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDef::SerializeToJson(writer);

	writer.String("m_NodeIDs");
	m_NodeIDs.SerializeToJson(writer);

	writer.String("m_bRandomSelectAtInit");
	writer.Bool(m_bRandomSelectAtInit);

	writer.String("m_bUpdateSelectIndex");
	writer.Bool(m_bUpdateSelectIndex);

	writer.EndObject();
}

ZNodeDefTransitionSelect::ZNodeDefTransitionSelect(unsigned int typeID) : MR::NodeDefPassThrough(typeID)
{
}

void ZNodeDefTransitionSelect::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThrough::SerializeToJson(writer);

	writer.String("m_currentVelocityCtrlID");
	m_currentVelocityCtrlID.SerializeToJson(writer);

	writer.String("m_currentFootCtrlID");
	m_currentFootCtrlID.SerializeToJson(writer);

	writer.String("m_targetVelocityCtrlID");
	m_targetVelocityCtrlID.SerializeToJson(writer);

	writer.String("m_targetRotationOffsetCtrlID");
	m_targetRotationOffsetCtrlID.SerializeToJson(writer);

	writer.String("m_targetPositionOffsetCtrlID");
	m_targetPositionOffsetCtrlID.SerializeToJson(writer);

	writer.String("m_sourceNodeIDs");
	writer.StartArray();

	for (unsigned int i = 0; i < 24; ++i)
	{
		m_sourceNodeIDs[i].SerializeToJson(writer);
	}

	writer.EndArray();

	writer.EndObject();
}

ZNodeDefIOITransit::ZNodeDefIOITransit(unsigned int typeID) : MR::NodeDefTransit(typeID)
{
}

void ZNodeDefIOITransit::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefTransit::SerializeToJson(writer, false);

	writer.String("m_bStartDestinationFromBeginning");
	writer.Bool(m_bStartDestinationFromBeginning);

	writer.String("m_bBlendFromLastFrame");
	writer.Bool(m_bBlendFromLastFrame);

	writer.EndObject();
}

NodeDefIOIDeltaAnim::NodeDefIOIDeltaAnim(unsigned int typeID) : MR::NodeDefBlend2(typeID)
{
}

NodeDefIOIDeltaAnimMatchEvents::NodeDefIOIDeltaAnimMatchEvents(unsigned int typeID) : MR::NodeDefBlend2MatchEvents(typeID)
{
}

MRSamples::NodeDefPlaySpeedModifier::NodeDefPlaySpeedModifier(unsigned int typeID) : MR::NodeDefPassThrough(typeID)
{
}

void MRSamples::NodeDefPlaySpeedModifier::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThrough::SerializeToJson(writer);

	writer.String("m_playSpeedFactorID");
	writer.Uint(m_playSpeedFactorID);

	writer.EndObject();
}

NodeDefIOIHeadController::NodeDefIOIHeadController(unsigned int typeID) : MR::NodeDefPassThroughTransforms(typeID)
{
}

void NodeDefIOIHeadController::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThroughTransforms::SerializeToJson(writer);

	writer.String("m_pointingVectorX");
	writer.Double(m_pointingVectorX);

	writer.String("m_pointingVectorY");
	writer.Double(m_pointingVectorY);

	writer.String("m_pointingVectorZ");
	writer.Double(m_pointingVectorZ);

	writer.String("m_offsetX");
	writer.Double(m_offsetX);

	writer.String("m_offsetY");
	writer.Double(m_offsetY);

	writer.String("m_offsetZ");
	writer.Double(m_offsetZ);

	writer.String("m_headEndJoint");
	m_headEndJoint.SerializeToJson(writer);

	writer.String("m_headRootJoint");
	m_headRootJoint.SerializeToJson(writer);

	writer.String("m_leftEyeJoint");
	m_leftEyeJoint.SerializeToJson(writer);

	writer.String("m_rightEyeJoint");
	m_rightEyeJoint.SerializeToJson(writer);

	writer.String("m_leftEyelidJoint");
	m_leftEyelidJoint.SerializeToJson(writer);

	writer.String("m_rightEyelidJoint");
	m_rightEyelidJoint.SerializeToJson(writer);

	writer.String("m_leftEyebrowJoint");
	m_leftEyebrowJoint.SerializeToJson(writer);

	writer.String("m_rightEyebrowJoint");
	m_rightEyebrowJoint.SerializeToJson(writer);

	writer.String("m_targetNodeID");
	m_targetNodeID.SerializeToJson(writer);

	writer.String("m_targetNoiseNodeID");
	m_targetNoiseNodeID.SerializeToJson(writer);

	writer.String("m_targetWeightNodeID");
	m_targetWeightNodeID.SerializeToJson(writer);

	writer.String("m_headBiasNodeID");
	m_headBiasNodeID.SerializeToJson(writer);

	writer.String("m_drowsinessNodeID");
	m_drowsinessNodeID.SerializeToJson(writer);

	writer.String("m_blendWeightNodeID");
	m_blendWeightNodeID.SerializeToJson(writer);

	writer.String("m_updateTargetByDeltas");
	writer.Bool(m_updateTargetByDeltas);

	writer.EndObject();
}

NodeDefIOIPelvisOffset::NodeDefIOIPelvisOffset(unsigned int typeID) : MR::NodeDefPassThroughTransforms(typeID)
{
}

void NodeDefIOIPelvisOffset::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThroughTransforms::SerializeToJson(writer);

	writer.String("m_OffsetWeightNodeID");
	writer.Uint(m_OffsetWeightNodeID);

	writer.String("m_OffsetVectorNodeID");
	writer.Uint(m_OffsetVectorNodeID);

	writer.String("m_pszBoneName");
	writer.String(m_pszBoneName);

	writer.EndObject();
}

void NodeDefIOIPelvisOffset::Locate()
{
	MR::NodeDef::Locate();

	m_pszBoneName += reinterpret_cast<uintptr_t>(this);
}

NodeDefIOIMirror::NodeDefIOIMirror(unsigned int typeID) : MR::NodeDefPassThroughTransforms(typeID)
{
}

void NodeDefIOIMirror::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThroughTransforms::SerializeToJson(writer);

	writer.String("m_nDisableOptionID");
	m_nDisableOptionID.SerializeToJson(writer);

	writer.String("m_bSwapHandAttachers");
	writer.Bool(m_bSwapHandAttachers);

	writer.EndObject();
}

NodeDefIOIMirrorMatchEvents::NodeDefIOIMirrorMatchEvents(unsigned int typeID) : MR::NodeDefPassThroughTransforms(typeID)
{
}

void NodeDefIOIMirrorMatchEvents::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThroughTransforms::SerializeToJson(writer);

	writer.String("m_rightFootEvent");
	writer.Uint(m_rightFootEvent);

	writer.String("m_leftFootEvent");
	writer.Uint(m_leftFootEvent);

	writer.String("m_rightHandEvent");
	writer.Uint(m_rightHandEvent);

	writer.String("m_leftHandEvent");
	writer.Uint(m_leftHandEvent);

	writer.String("m_bSwapHandAttachers");
	writer.Bool(m_bSwapHandAttachers);

	writer.EndObject();
}

NodeDefIOIBlend2AndReTarget::NodeDefIOIBlend2AndReTarget(unsigned int typeID) : MR::NodeDefBlend2(typeID)
{
}

void NodeDefIOIBlend2AndReTarget::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefBlend2::SerializeToJson(writer, false);

	writer.String("m_nReTargetFlags");
	writer.Uint(m_nReTargetFlags);

	writer.EndObject();
}

NodeDefPlaySpeedModifierMatchEvents::NodeDefPlaySpeedModifierMatchEvents(unsigned int typeID) : MR::NodeDefPassThrough(typeID)
{
}

void NodeDefPlaySpeedModifierMatchEvents::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThrough::SerializeToJson(writer);

	writer.String("m_playSpeedFactorID");
	writer.Uint(m_playSpeedFactorID);

	writer.EndObject();
}

NodeDefIOIPartialMirrorAim::NodeDefIOIPartialMirrorAim(unsigned int typeID) : MR::NodeDefPassThroughTransforms(typeID)
{
}

void NodeDefIOIPartialMirrorAim::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThroughTransforms::SerializeToJson(writer);

	writer.String("m_MirrorWeightNodeID");
	writer.Uint(m_MirrorWeightNodeID);

	writer.EndObject();
}

NodeDefBlend2ForwardEvents::NodeDefBlend2ForwardEvents(unsigned int typeID) : MR::NodeDefBlend2(typeID)
{
}

void NodeDefBlend2ForwardEvents::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefBlend2::SerializeToJson(writer, false);

	writer.String("m_bUseGlobalDeltaTime");
	writer.Bool(m_bUseGlobalDeltaTime);

	writer.EndObject();
}

ZNodeDefIOISneakModifier::ZNodeDefIOISneakModifier(unsigned int typeID) : MR::NodeDefPassThroughTransforms(typeID)
{
}

void ZNodeDefIOISneakModifier::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThroughTransforms::SerializeToJson(writer);

	writer.String("m_nWeightNodeID");
	writer.Uint(m_nWeightNodeID);

	writer.String("m_nPelvisOffsetNodeID");
	writer.Uint(m_nPelvisOffsetNodeID);

	writer.String("m_nPelvisAngleNodeID");
	writer.Uint(m_nPelvisAngleNodeID);

	writer.String("m_nSpeedModifierNodeID");
	writer.Uint(m_nSpeedModifierNodeID);

	writer.String("m_bUseGlobalDeltaTime");
	writer.Uint(m_nLeftHandQuatNodeID);

	writer.EndObject();
}

ZNodeDefIOITranslationScale::ZNodeDefIOITranslationScale(unsigned int typeID) : MR::NodeDefPassThrough(typeID)
{
}

void ZNodeDefIOITranslationScale::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	MR::NodeDefPassThrough::SerializeToJson(writer);

	writer.String("m_nScaleNodeId");
	writer.Uint(m_nScaleNodeId);

	writer.EndObject();
}

ZNodeDefIOIWildCardTransitMatchEvents::ZNodeDefIOIWildCardTransitMatchEvents(unsigned int typeID) : MR::NodeDefTransitMatchEvents(typeID)
{
}

ZNodeDefIOIWildCardTransit::ZNodeDefIOIWildCardTransit(unsigned int typeID) : MR::NodeDefTransit(typeID)
{
}

void SNodeSelectMatchEventsInfo::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	writer.String("m_sourceNodeIDs");
	writer.StartArray();

	for (unsigned int i = 0; i < 24; ++i)
	{
		m_sourceNodeIDs[i].SerializeToJson(writer);
	}

	writer.EndArray();

	writer.String("m_selectCtrlID");
	writer.Uint(m_selectCtrlID);

	writer.String("m_blendCtrlID");
	writer.Uint(m_blendCtrlID);

	writer.EndObject();
}

void SNodeSelectMatchEventsInfo::SNodeInfo::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	writer.String("m_id");
	writer.Uint(m_id);

	writer.String("m_index");
	writer.Double(m_id);

	writer.EndObject();
}

ZNodeDefSelectMatchEvents::ZNodeDefSelectMatchEvents(unsigned int typeID) : MR::NodeDefTransitMatchEvents(typeID)
{
}

void ZNodeDefSelectMatchEvents::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject) const
{
	writer.StartObject();

	writer.String("m_NodeIDs");
	m_NodeIDs.SerializeToJson(writer);

	writer.String("m_bRandomSelectAtInit");
	writer.Bool(m_bRandomSelectAtInit);

	writer.String("m_bUpdateSelectIndex");
	writer.Bool(m_bUpdateSelectIndex);

	writer.EndObject();
}
