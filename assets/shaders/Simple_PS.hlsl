static const float4 meshColor = { 0.721f, 0.709f, 0.709f, 1.0f };

float4 mainPS() : SV_Target
{
    return meshColor;
}

