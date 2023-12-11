#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "EActorPerceptionSensitivity.h"

class ZBinarySerializer;

class ZSharedSensorDef
{
public:
    struct SVisibilitySetting
    {
        void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
        void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
        static SVisibilitySetting* DeserializeFromJson(const rapidjson::Value& object);
        bool operator==(const SVisibilitySetting& other);

        EActorPerceptionSensitivity m_eSensitivity;
        float m_fCloseRange;
        float m_fPeripheralRange;
        float m_fPeripheralAngle;
        float m_fPeripheralHigh;
        float m_fPeripheralLow;
        float m_fFocusConeWidthAngle;
        float m_fFocusConeHeightAngle;
        float m_fFocusConeRange;
    };
};
