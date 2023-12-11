#pragma once

#include <unordered_map>
#include <string>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "ECustomDataType.h"
#include "../Math/SMatrix43.h"

namespace NMP
{
	class StringTable
	{
	public:
		void Locate();
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true);

	private:
		unsigned int m_NumEntrys;
		unsigned int m_DataLength;
		unsigned int* m_IDs;
		unsigned int* m_Offsets;
		const char* m_Data;
	};

	class Vector3
	{
	public:
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

	private:
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};

			float f[4];
		};
	};

	class Quat
	{
	public:
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

	private:
		float x;
		float y;
		float z;
		float w;
	};

	template <typename T>
	class Flags
	{
	public:
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const
		{
			writer.StartObject();

			writer.Uint(m_flags);

			writer.EndObject();
		}

	private:
		T m_flags;
	};

	class Hierarchy
	{
	public:
		void Locate();
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true);

	private:
		unsigned int m_NumEntries;
		int* m_HierarchyArray;
	};
}

namespace MR
{
	class NodeDef;
	class TransitConditionDef;

	class Manager
	{
	public:
		~Manager();
		static Manager& GetInstance();
		void Construct(TransitConditionDef* transitConditionDef);
		void Construct(NodeDef* nodeDef);

	private:
		Manager();
		Manager(const Manager& other) = delete;
		Manager& operator=(const Manager& other) = delete;

		NodeDef* nodeTypes[54];
		TransitConditionDef* transitConditionTypes[11];
	};

	namespace Operator2Float
	{
		enum class OperationType
		{
			OPERATION_MULTIPLY = 0,
			OPERATION_ADD = 1,
			OPERATION_DIVIDE = 2,
			OPERATION_SUBTRACT = 3,
			OPERATION_MIN = 4,
			OPERATION_MAX = 5,
			OPERATION_NUMOPS = 6,
			OPERATION_FORCEINTSIZE = -1
		};

		std::string ConvertOperationTypeToString(OperationType operationType);
	}

	class TransitConditionDef
	{
	public:
		TransitConditionDef(unsigned int nTypeId);
		virtual void Locate() = 0;
		unsigned int GetType() const;
		virtual void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true);

	private:
		unsigned int m_type;
		unsigned int m_offset;
	};

	class TransitConditionDefFalse : public TransitConditionDef
	{
	public:
		TransitConditionDefFalse(unsigned int nTypeId);
		void Locate() override;
	};

	class TransitConditionDefOnRequest : public TransitConditionDef
	{
	public:
		TransitConditionDefOnRequest(unsigned int nTypeId);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) override;

	private:
		unsigned int m_requestID;
		bool m_onNotSet;
	};

	class TransitConditionDefInEventSegment : public TransitConditionDef
	{
	public:
		TransitConditionDefInEventSegment(unsigned int nTypeId);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) override;

	private:
		unsigned int m_eventSegmentIndex;
	};

	class TransitConditionDefCrossedEventBoundary : public TransitConditionDef
	{
	public:
		TransitConditionDefCrossedEventBoundary(unsigned int nTypeId);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) override;

	private:
		float m_eventTriggerPosition;
	};

	class TransitConditionDefCrossedDurationFraction : public TransitConditionDef
	{
	public:
		TransitConditionDefCrossedDurationFraction(unsigned int nTypeId);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) override;

	private:
		float m_durationFractionTrigger;
	};

	class TransitConditionDefControlParamFloatGreater : public TransitConditionDef
	{
	public:
		TransitConditionDefControlParamFloatGreater(unsigned int nTypeId);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) override;

	private:
		unsigned int m_controlParamNodeID;
		float m_testValue;
		bool m_orEqual;
	};

	class TransitConditionDefControlParamFloatLess : public TransitConditionDef
	{
	public:
		TransitConditionDefControlParamFloatLess(unsigned int nTypeId);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) override;

	private:
		unsigned int m_controlParamNodeID;
		float m_testValue;
		bool m_orEqual;
	};

	class TransitConditionDefControlParamFloatInRange : public TransitConditionDef
	{
	public:
		TransitConditionDefControlParamFloatInRange(unsigned int nTypeId);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) override;

	private:
		unsigned int m_controlParamNodeID;
		float m_lowerTestValue;
		float m_upperTestValue;
		bool m_notInRange;
	};

	class TransitConditionDefDiscreteEventTriggered : public TransitConditionDef
	{
	public:
		TransitConditionDefDiscreteEventTriggered(unsigned int nTypeId);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) override;

	private:
		unsigned int m_eventUserData;
		float m_eventWeightThreshold;
		bool m_orEqual;
	};

	class TransitConditionDefInEventRange : public TransitConditionDef
	{
	public:
		TransitConditionDefInEventRange(unsigned int nTypeId);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) override;

	private:
		float m_eventRangeStart;
		float m_eventRangeEnd;
	};

	class TransitConditionDefPercentageThroughSubtransition : public TransitConditionDef
	{
	public:
		TransitConditionDefPercentageThroughSubtransition(unsigned int nTypeId);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) override;

	private:
		float m_fPercentageThrough;
		unsigned int m_nSubStateID;
		bool m_bUseTransitionSource;
		bool m_bLessThan;
	};

	class BufferableDataType;
	class Node;
	class Network;

	class NodeDef
	{
	public:
		NodeDef(unsigned int typeID);
		virtual void Locate();
		unsigned int GetTypeID() const;
		virtual void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

	private:
		unsigned int m_typeID;
		unsigned int m_nodeID;
		const char* m_pName;
		const NodeDef* m_pParent;
	};

	class NodeDefControlParam : public NodeDef
	{
	public:
		NodeDefControlParam(unsigned int typeID);
	};

	class NodeDefControlParamFloat : public NodeDefControlParam
	{
	public:
		NodeDefControlParamFloat(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		float m_defaultVal;
	};

	class NodeDefControlParamVector3 : public NodeDefControlParam
	{
	public:
		NodeDefControlParamVector3(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned char m_pad[12];
		NMP::Vector3 m_defaultVal;
	};

	class NodeDefControlParamVector4 : public NodeDefControlParam
	{
	public:
		NodeDefControlParamVector4(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned char m_pad[12];
		NMP::Quat m_defaultVal;
	};

	class NodeDefOperatorFloat : public NodeDefControlParam
	{
	public:
		NodeDefOperatorFloat(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_input;
		Operator2Float::OperationType m_operation;
	};

	class NodeDefOperator2Float : public NodeDefControlParam
	{
	public:
		NodeDefOperator2Float(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_input1;
		unsigned int m_input2;
		Operator2Float::OperationType m_operation;
	};

	class NodeDefOperatorConst : public NodeDefControlParam
	{
	public:
		NodeDefOperatorConst(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_input;
		Operator2Float::OperationType m_operation;
		float m_constValue;
	};

	class NodeDefOperatorNoiseGen : public NodeDefControlParam
	{
	public:
		NodeDefOperatorNoiseGen(unsigned int typeID);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_input;
		float m_frequency;
		unsigned int m_noiseFlags;
		float* m_table1;
		float* m_table2;
		float* m_table3;
	};

	class NodeDefPassThrough : public NodeDef
	{
	public:
		NodeDefPassThrough(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_nSourceNodeId;
	};

	class StateChangeDef
	{
	public:
		void Locate();
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

	private:
		unsigned int m_destinationStateID;
		unsigned int m_numConditions;
		unsigned int* m_conditionIndexes;
	};

	class StateDef
	{
	public:
		void Locate();
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

	private:
		unsigned int m_nodeID;
		unsigned int m_numConditions;
		unsigned int* m_conditionIndexes;
		unsigned int m_numStateChanges;
		StateChangeDef* m_stateChanges;
	};

	class NodeDefStateMachine : public NodeDefPassThrough
	{
	public:
		NodeDefStateMachine(unsigned int typeID);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_numStates;
		StateDef* m_stateDefs;
		unsigned int m_numConditions;
		TransitConditionDef** m_conditions;
		unsigned int m_numChildren;
		NodeDef** m_children;
	};

	class IAnimSource;
	
	class Rig
	{
	public:
		void Locate();
		std::string SerializeToJson();

	private:
		NMP::Quat m_blendFrameOrientation;
		NMP::Hierarchy* m_hierarchy;
		unsigned int m_trajectoryBoneIndex;
		unsigned int m_characterRootBoneIndex;
		NMP::StringTable* m_boneNameMap;
		NMP::Quat* m_boneQuat;
		NMP::Vector3* m_bonePos;
		unsigned int m_spuMemoryRequirements;
		unsigned int* m_aGlobalIDToRigID;
		unsigned int m_nGlobalIDToRigIDCount;
	};

	class NodeDefAnim : public NodeDef
	{
	public:
		NodeDefAnim(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		bool m_generateDeltas;
		bool m_loop;
		bool m_playBackwards;
		float m_clipStartFraction;
		float m_clipEndFraction;
		IAnimSource* m_animSource;
	};

	class NodeDefAnimWithEvents : public NodeDefAnim
	{
	public:
		NodeDefAnimWithEvents(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		int m_startEventIndex;
	};

	class NodeDefBlendBase : public NodeDef
	{
	public:
		NodeDefBlendBase(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_aID;
		unsigned int m_bID;
		bool m_UseSource1Additively;
	};

	class NodeDefBlend2 : public NodeDefBlendBase
	{
	public:
		NodeDefBlend2(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_weightNodeID;
		bool m_loop;
		bool m_additiveBlendAtt;
		bool m_additiveBlendPos;
		bool m_slerpDeltaTrajTrans;
		bool m_source0BranchOptimization;
		bool m_source1BranchOptimization;
		bool m_ignoreSource0EventWeight;
		bool m_ignoreSource1EventWeight;
		bool m_bUpdateWeigthingOnce;
	};

	class NodeDefBlendN : public NodeDefBlend2
	{
	public:
		class ConnectionDef
		{
		public:
			void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

		private:
			unsigned int connectedNodeId;
			float weight;
		};

		NodeDefBlendN(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_numSourceNodes;
		ConnectionDef m_connectedSourceNode[1];
	};

	class NodeDefBlend2MatchEvents : public NodeDefBlend2
	{
	public:
		NodeDefBlend2MatchEvents(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_eventWeightNodeID;
		int m_startEventIndex;
		unsigned int m_durationEventMatchingType;
	};

	class NodeDefBlendNMatchEvents : public NodeDefBlend2MatchEvents
	{
	public:
		class ConnectionDef
		{
		public:
			void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

		private:
			unsigned int connectedNodeId;
			float weight;
		};

		NodeDefBlendNMatchEvents(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_numSourceNodes;
		ConnectionDef m_connectedSourceNode[1];
	};

	class NodeDefPassThroughTransforms : public NodeDefPassThrough
	{
	public:
		NodeDefPassThroughTransforms(unsigned int typeID);
	};

	class NodeDefFilterTransforms : public NodeDefPassThroughTransforms
	{
	public:
		NodeDefFilterTransforms(unsigned int typeID);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_numBoneFilterIDs;
		unsigned int* m_boneFilterIDs;
	};

	class NodeDefSingleFrame : public NodeDefPassThroughTransforms
	{
	public:
		NodeDefSingleFrame(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_controlNodeID;
		bool m_bUseTrajectory;
	};

	class NodeDefComputeWorldTransforms : public NodeDefPassThroughTransforms
	{
	public:
		NodeDefComputeWorldTransforms(unsigned int typeID);
	};

	struct NodeRef
	{
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

		unsigned short m_nodeId;
	};

	class NodeDefFeatherBlend2 : public NodeDef
	{
	public:
		NodeDefFeatherBlend2(unsigned int typeID);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		NodeRef m_aID;
		NodeRef m_bID;
		NodeRef m_weightNodeID;
		unsigned short m_numBoneWeights;
		float* m_boneWeights;
		bool m_loop;
		bool m_additiveBlendAtt;
		bool m_additiveBlendPos;
		bool m_slerpDeltaTrajTrans;
	};

	struct BoneId
	{
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

		unsigned short m_boneId;
	};

	class NodeDefLockFoot : public NodeDefPassThroughTransforms
	{
	public:
		NodeDefLockFoot(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		float m_catchupSpeedFactor;
		float m_snapToSourceDistance;
		float m_straightestLegFactor;
		int m_footfallEventID;
		float m_lowerHeightBound;
		float m_toeLowerHeightBound;
		NMP::Vector3 m_kneeRotationAxis;
		NMP::Vector3 m_ballRotationAxis;
		NMP::Flags<unsigned int> m_upAxis;
		NodeRef m_IkFkBlendWeightNodeID;
		NodeRef m_swivelContributionToOrientationNodeID;
		unsigned short m_pad;
		BoneId m_hipIndex;
		BoneId m_kneeIndex;
		BoneId m_ankleIndex;
		BoneId m_ballIndex;
		BoneId m_toeIndex;
		bool m_fixGroundPenetration;
		bool m_flipKneeRotationDirection;
		bool m_useBallJoint;
		bool m_assumeSimpleHierarchy;
		bool m_fixToeGroundPenetration;
		bool m_valid;
	};

	class NodeDefTwoBoneIK : public NodeDefPassThroughTransforms
	{
	public:
		NodeDefTwoBoneIK(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		BoneId m_endJointIndex;
		BoneId m_midJointIndex;
		BoneId m_rootJointIndex;
		BoneId m_referenceJointIndex;
		NodeRef m_effectorTargetNodeID;
		NodeRef m_targetOrientationNodeID;
		NodeRef m_swivelAngleNodeID;
		NodeRef m_IkFkBlendWeightNodeID;
		NodeRef m_swivelContributionToOrientationNodeID;
		float m_midJointRotationAxisX;
		float m_midJointRotationAxisY;
		float m_midJointRotationAxisZ;
		bool m_flipMidJointRotationDirection;
		bool m_assumeSimpleHierarchy;
		bool m_keepEndEffOrientation;
		bool m_endEffOrientationFromReferenceJoint;
		bool m_globalReferenceAxis;
		bool m_updateTargetByDeltas;
		float m_midJointReferenceAxisX;
		float m_midJointReferenceAxisY;
		float m_midJointReferenceAxisZ;
	};

	class NodeDefHeadLook : public NodeDefPassThroughTransforms
	{
	public:
		NodeDefHeadLook(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		BoneId m_endJointIndex;
		BoneId m_rootJointIndex;
		BoneId m_pointingJointIndex;
		NodeRef m_targetNodeID;
		NodeRef m_pointingJointAxisNodeID;
		NodeRef m_pointingJointIDNodeID;
		NodeRef m_blendWeightNodeID;
		float m_pointingJointAxisX;
		float m_pointingJointAxisY;
		float m_pointingJointAxisZ;
		float m_bias;
		bool m_updateTargetByDeltas;
		bool m_pointingVectorInCS;
	};

	class NodeDefApplyBindPose : public NodeDefPassThroughTransforms
	{
	public:
		NodeDefApplyBindPose(unsigned int typeID);
	};

	class NodeDefTransit : public NodeDefBlendBase
	{
	public:
		NodeDefTransit(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		float m_transitionDuration;
		bool m_bIgnoreSourceTrajectory;
		bool m_bEnableRealign;
	};

	class NodeDefTransitAtEvent : public NodeDefTransit
	{
	public:
		NodeDefTransitAtEvent(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_destStartEventIndex;
		bool m_startFromSetFractionOfDestEvent;
		float m_destStartEventFraction;
	};

	class NodeDefTransitMatchEvents : public NodeDefTransit
	{
	public:
		NodeDefTransitMatchEvents(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_destEventSequenceOffset;
		unsigned int m_destStartEventIndex;
		bool m_usingDestStartEventIndex;
		unsigned int m_durationEventMatchingType;
	};

	class NodeDefBlendTree : public NodeDef
	{
	public:
		NodeDefBlendTree(unsigned int typeID);
		void Locate() override;
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_nNumChildren;
		NodeDef** m_children;
		unsigned int m_nRootNodeId;
	};

	class TransformBuffersCount
	{
	public:
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true);

	private:
		unsigned int m_numTranformBufferTypesUsed;
		unsigned int m_transformBufferTypes[8];
		unsigned int m_transformBufferCounts[8];
	};

	class NetworkDef
	{
	public:
		void Locate();
		std::string SerializeToJson();

		struct _NetworkHash
		{
			unsigned char b[16];
		};

	private:
		unsigned int m_numAnimations;
		IAnimSource** m_animLibrary;
		Rig* m_rig;
		unsigned int m_numNodeNames;
		const char* m_nodeNames;
		unsigned int m_numNodeDefs;
		unsigned int m_numDummyBlendTreeDefs;
		NodeDef** m_nodeDefs;
		unsigned int m_maxDurationEventsPoolSize;
		unsigned int m_rootNodeID;
		unsigned int m_numNodeStateMachines;
		unsigned int* m_nodeStateMachineNodeIDs;
		unsigned int m_numInterfaceControlParameters;
		unsigned int* m_interfaceControlParameterNodeIDs;
		unsigned int m_numPersistentNodes;
		unsigned int* m_persistentNodeIDs;
		_NetworkHash m_networkHash;
		NMP::StringTable* m_requestIDStringTable;
		TransformBuffersCount m_maxTransformBufferReqs;
	};

	class EventsSequenceDefBase
	{
	public:
		void Locate();

	protected:
		unsigned int m_numEvents;
		unsigned int m_type;
		char* m_name;
		unsigned int m_userData;
	};

	class TimeFraction
	{
	public:
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	private:
		float m_value;
	};

	class EventDef
	{
	public:
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, ECustomDataType customDataType, unsigned int* data);
		std::string ConvertCustomDataTypeToString(const ECustomDataType customDataType);

	private:
		TimeFraction m_startTime;
		TimeFraction m_duration;
		unsigned int m_userData;
	};

	class EventsSequenceDef : public EventsSequenceDefBase
	{
	public:
		void Locate();
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, unsigned int* customData);

	private:
		EventDef* m_events;
	};

	class EventDefDuration
	{
	public:
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	private:
		TimeFraction m_startTime;
		TimeFraction m_duration;
		unsigned int m_userData;
	};

	class EventsSequenceDefDuration : public EventsSequenceDefBase
	{
	public:
		void Locate();
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, unsigned int* customData);

	private:
		EventDefDuration* m_events;
	};

	struct EventSequenceHeader
	{
		void Locate();
		std::string SerializeToJson();
		std::string ConvertCustomDataTypeToString(const ECustomDataType customDataType);

		unsigned int m_timeStretchEventSequenceIndex;
		unsigned int m_numEventSequences;
		EventsSequenceDef** m_eventSequences;
		unsigned int** m_customData;
		unsigned int m_numDurationEventSequences;
		EventsSequenceDefDuration** m_durationEventSequences;
	};
}

class ZNodeDefAnimIOI : public MR::NodeDefAnimWithEvents
{
public:
	ZNodeDefAnimIOI(unsigned int typeID);
};

class ZNodeDefAnimIOICutSequence : public MR::NodeDef
{
public:
	ZNodeDefAnimIOICutSequence(unsigned int typeID);
	void Locate() override;
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	const char* m_pszSlotName;
};

class ZNodeDefVariationIOI : public ZNodeDefAnimIOI
{
public:
	ZNodeDefVariationIOI(unsigned int typeID);
	void Locate() override;
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	const char* m_pszVariationName;
	bool m_bBlendFromLastFrame;
};

class ZNodeDefChildNetworkIOI : public MR::NodeDef
{
public:
	ZNodeDefChildNetworkIOI(unsigned int typeID);
};

class ZNodeDefSelectiveFeatherBlend : public MR::NodeDefBlend2
{
public:
	ZNodeDefSelectiveFeatherBlend(unsigned int typeID);
	void Locate() override;
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	unsigned int m_featherWeightNodeID;
	unsigned int m_numBoneWeights;
	float* m_paBoneWeights;
};

class ZNodeDefGroundRotation : public MR::NodeDefPassThrough
{
public:
	ZNodeDefGroundRotation(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	unsigned int m_nAngleNodeId;
	unsigned int m_nTrajectoryNodeID;
	unsigned int m_nTranslationThresholdNodeID;
	unsigned int m_nAttitudeThresholdNodeID;
	bool m_bOverrideTrajectory;
	bool m_bCorrectiveTranslationModification;
	bool m_bCorrectiveAttitudeModification;
	bool m_bRotateTrajectory;
	bool m_bRotateTrajectoryToEndPosition;
	bool m_bApplyAsDeltaValue;
	bool m_bMirrorTrajectory;
	unsigned int m_nCorrectionStartEventID;
	unsigned int m_nCorrectionEndEventID;
};

class ZNodeDefScaleAnim : public MR::NodeDefPassThroughTransforms
{
public:
	ZNodeDefScaleAnim(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	unsigned int m_nBoneScaleNodeId;
	unsigned int m_nTrajectoryTranslationScaleNodeID;
	unsigned int m_nTrajectoryOrientationScaleNodeID;
	unsigned int m_nTimeScaleNodeId;
	MR::NodeRef m_nReferenceNodeId;
};

struct SNodeLookAtIDs
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

	unsigned int m_directionNodeID;
	unsigned int m_weightNodeID;
	unsigned int m_iHeadBoneIndex;
};

class ZIOINodeLookAtDef : public MR::NodeDefPassThroughTransforms
{
public:
	ZIOINodeLookAtDef(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	SNodeLookAtIDs m_NodeIDs;
};

struct SNodeSelectIDs
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

	unsigned int m_sourceNodeIDs[24];
	unsigned int m_selectCtrlID;
	unsigned int m_blendCtrlID;
};

class ZIOINodeSelectDef : public MR::NodeDef
{
public:
	ZIOINodeSelectDef(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	SNodeSelectIDs m_NodeIDs;
	bool m_bRandomSelectAtInit;
	bool m_bUpdateSelectIndex;
};

class ZNodeDefTransitionSelect : public MR::NodeDefPassThrough
{
public:
	ZNodeDefTransitionSelect(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	MR::NodeRef m_currentVelocityCtrlID;
	MR::NodeRef m_currentFootCtrlID;
	MR::NodeRef m_targetVelocityCtrlID;
	MR::NodeRef m_targetRotationOffsetCtrlID;
	MR::NodeRef m_targetPositionOffsetCtrlID;
	MR::NodeRef m_sourceNodeIDs[24];
};

class ZNodeDefIOITransit : public MR::NodeDefTransit
{
public:
	ZNodeDefIOITransit(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	bool m_bStartDestinationFromBeginning;
	bool m_bBlendFromLastFrame;
};

class NodeDefIOIDeltaAnim : public MR::NodeDefBlend2
{
public:
	NodeDefIOIDeltaAnim(unsigned int typeID);
};

class NodeDefIOIDeltaAnimMatchEvents : public MR::NodeDefBlend2MatchEvents
{
public:
	NodeDefIOIDeltaAnimMatchEvents(unsigned int typeID);
};

namespace MRSamples
{
	class NodeDefPlaySpeedModifier : public MR::NodeDefPassThrough
	{
	public:
		NodeDefPlaySpeedModifier(unsigned int typeID);
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

	private:
		unsigned int m_playSpeedFactorID;
	};
}

class NodeDefIOIHeadController : public MR::NodeDefPassThroughTransforms
{
public:
	NodeDefIOIHeadController(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	float m_pointingVectorX;
	float m_pointingVectorY;
	float m_pointingVectorZ;
	float m_offsetX;
	float m_offsetY;
	float m_offsetZ;
	MR::BoneId m_headEndJoint;
	MR::BoneId m_headRootJoint;
	MR::BoneId m_leftEyeJoint;
	MR::BoneId m_rightEyeJoint;
	MR::BoneId m_leftEyelidJoint;
	MR::BoneId m_rightEyelidJoint;
	MR::BoneId m_leftEyebrowJoint;
	MR::BoneId m_rightEyebrowJoint;
	MR::NodeRef m_targetNodeID;
	MR::NodeRef m_targetNoiseNodeID;
	MR::NodeRef m_targetWeightNodeID;
	MR::NodeRef m_headBiasNodeID;
	MR::NodeRef m_drowsinessNodeID;
	MR::NodeRef m_blendWeightNodeID;
	bool m_updateTargetByDeltas;
};

class NodeDefIOIPelvisOffset : public MR::NodeDefPassThroughTransforms
{
public:
	NodeDefIOIPelvisOffset(unsigned int typeID);
	void Locate() override;
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	unsigned int m_OffsetWeightNodeID;
	unsigned int m_OffsetVectorNodeID;
	const char* m_pszBoneName;
};

class NodeDefIOIMirror : public MR::NodeDefPassThroughTransforms
{
public:
	NodeDefIOIMirror(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	MR::NodeRef m_nDisableOptionID;
	bool m_bSwapHandAttachers;
};

class NodeDefIOIMirrorMatchEvents : public MR::NodeDefPassThroughTransforms
{
public:
	NodeDefIOIMirrorMatchEvents(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	unsigned int m_rightFootEvent;
	unsigned int m_leftFootEvent;
	unsigned int m_rightHandEvent;
	unsigned int m_leftHandEvent;
	bool m_bSwapHandAttachers;
};

class NodeDefIOIBlend2AndReTarget : public MR::NodeDefBlend2
{
public:
	NodeDefIOIBlend2AndReTarget(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	unsigned int m_nReTargetFlags;
};

class NodeDefPlaySpeedModifierMatchEvents : public MR::NodeDefPassThrough
{
public:
	NodeDefPlaySpeedModifierMatchEvents(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	unsigned int m_playSpeedFactorID;
};

class NodeDefIOIPartialMirrorAim : public MR::NodeDefPassThroughTransforms
{
public:
	NodeDefIOIPartialMirrorAim(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	unsigned int m_MirrorWeightNodeID;
};

class NodeDefBlend2ForwardEvents : public MR::NodeDefBlend2
{
public:
	NodeDefBlend2ForwardEvents(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	bool m_bUseGlobalDeltaTime;
};

class ZNodeDefIOISneakModifier : public MR::NodeDefPassThroughTransforms
{
public:
	ZNodeDefIOISneakModifier(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	unsigned int m_nWeightNodeID;
	unsigned int m_nPelvisOffsetNodeID;
	unsigned int m_nPelvisAngleNodeID;
	unsigned int m_nSpeedModifierNodeID;
	unsigned int m_nLeftHandQuatNodeID;
};

class ZNodeDefIOITranslationScale : public MR::NodeDefPassThrough
{
public:
	ZNodeDefIOITranslationScale(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	unsigned int m_nScaleNodeId;
};

class ZNodeDefIOIWildCardTransitMatchEvents : public MR::NodeDefTransitMatchEvents
{
public:
	ZNodeDefIOIWildCardTransitMatchEvents(unsigned int typeID);
};

class ZNodeDefIOIWildCardTransit : public MR::NodeDefTransit
{
public:
	ZNodeDefIOIWildCardTransit(unsigned int typeID);
};

struct SNodeSelectMatchEventsInfo
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

	struct SNodeInfo
	{
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const;

		unsigned int m_id;
		float m_index;
	};

	SNodeInfo m_sourceNodeIDs[24];
	unsigned int m_selectCtrlID;
	unsigned int m_blendCtrlID;
};

class ZNodeDefSelectMatchEvents : public MR::NodeDefTransitMatchEvents
{
public:
	ZNodeDefSelectMatchEvents(unsigned int typeID);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const bool startObject = true) const override;

private:
	SNodeSelectMatchEventsInfo m_NodeIDs;
	bool m_bRandomSelectAtInit;
	bool m_bUpdateSelectIndex;
};

struct SSequenceOriginInfo
{
	SMatrix43 m_mSequenceOrigin;
};
