#include "Common.hlsl"

struct PS_INPUT
{
    float3 viewPosition : POSITION;
	//matrix sphereMatrix : ATTRIBUTE0;
};

struct PS_OUTPUT
{
    float4 fragColor : SV_Target;
    float depth : SV_Depth;
};

float get_depth_from_view_z(float z)
{
	float d;
	
	if (projection[3][3] == 0.0)
	{
		d = (-projection[3][2] / z) - projection[2][2];
	}
	else
	{
		d = projection[2][2] * z + projection[3][2];
	}
	
	return d * 0.5 + 0.5;
}

float4 mainPS(const PS_INPUT input) : SV_Target
{
	PS_OUTPUT output;
    const float sphere_radius = 0.05;

    bool is_perp = (projection[3][3] == 0.0);
    float3 ray_ori_view = is_perp ? float3(0.0, 0.0, 0.0) : input.viewPosition.xyz;
    float3 ray_dir_view = is_perp ? input.viewPosition : float3(0.0, 0.0, -1.0);

    /* Single matrix mul without branch. */
    float4 mul_vec = is_perp ? float4(ray_dir_view, 0.0) : float4(ray_ori_view, 1.0);
    float3 mul_res = (mul(mul_vec, sphereMatrix)).xyz;

    /* Reminder :
    * sphereMatrix[3] is the view space origin in sphere space (sph_ori -> view_ori).
    * sphereMatrix[2] is the view space Z axis in sphere space. */

    /* convert to sphere local space */
    float3 ray_ori = (is_perp) ? sphereMatrix[3].xyz : mul_res;
    float3 ray_dir = (is_perp) ? mul_res : -sphereMatrix[2].xyz;
    float ray_len = length(ray_dir);
    ray_dir /= ray_len;

    /* Line to sphere intersect */
    const float sphere_radius_sqr = sphere_radius * sphere_radius;
    float b = dot(ray_ori, ray_dir);
    float c = dot(ray_ori, ray_ori) - sphere_radius_sqr;
    float h = b * b - c;
    float t = -sqrt(max(0.0, h)) - b;

    /* Compute dot product for lighting */
    float3 p = ray_dir * t + ray_ori; /* Point on sphere */
    float3 n = normalize(p);          /* Normal is just the point in sphere space, normalized. */
    float3 l = normalize(sphereMatrix[2].xyz); /* Just the view Z axis in the sphere space. */

    /* Smooth lighting factor. */
    const float s = 0.2; /* [0.0-0.5] range */
    float fac = saturate((dot(n, l) * (1.0 - s)) + s);
	float3 finalBoneColor = float3(0.445201248f, 0.445201248f, 0.445201248f);
	float3 finalStateColor = float3(0.0198204145f, 0.0198204145f, 0.0198204145f);
    output.fragColor.rgb = lerp(finalStateColor, finalBoneColor, fac * fac);

    /* 2x2 dither pattern to smooth the lighting. */
    //float dither = (0.5 + dot(fmod(float2(gl_FragCoord.xy), float2(2.0)), float2(1.0, 2.0))) * 0.25;
    //dither *= (1.0 / 255.0); /* Assume 8bit per color buffer. */

    //output.fragColor = float4(output.fragColor.rgb + dither, alpha);
	output.fragColor = float4(output.fragColor.rgb, 1.0f);

    //t /= ray_len;
    //output.depth = get_depth_from_view_z(ray_dir_view.z * t + ray_ori_view.z);
	
	return output.fragColor;
}