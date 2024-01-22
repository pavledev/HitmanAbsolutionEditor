cbuffer ChannelControlBuffer : register(b1)
{
    bool isRedChannelEnabled;
	bool isGreenChannelEnabled;
	bool isBlueChannelEnabled;
	bool isAlphaChannelEnabled;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

sampler sampler0;
Texture2D texture0;
            
float4 mainPS(PS_INPUT input) : SV_Target
{
	float4 outputColor = input.col * texture0.Sample(sampler0, input.uv);
	
	if (!isRedChannelEnabled)
	{
        outputColor.r = 0;
    }
	
    if (!isGreenChannelEnabled)
	{
        outputColor.g = 0;
    }
	
    if (!isBlueChannelEnabled)
	{
        outputColor.b = 0;
    }
	
    if (!isAlphaChannelEnabled)
	{
        outputColor.a = 1;
    }
	
	return outputColor;
}