void ReplicateChannel(inout float4 baseColor, float4 componentReplicate, float4 componentReplicateAlpha)
{
#if FEATURE_LEVEL >= FEATURE_LEVEL_SM4
	//In SM4/5, doing a texture lookup from a G8 texture gives float4(value,0,0,0), so we need to replicate it to the other channels.  
	//On all other platforms, the value is replicated automatically.
	if(any(componentReplicate != 0.0))
	{
		baseColor = dot(baseColor, componentReplicate);
	}
#endif
	baseColor.a = dot(baseColor, componentReplicateAlpha);
}
