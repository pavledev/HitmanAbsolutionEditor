#include "Common.hlsl"

struct PSInput
{
	float4 position : SV_POSITION;
    float4 eye : POSITION0;
	float4 worldPosition : POSITION1;
    float3 normal : NORMAL;
	float2 textureCoordinates : TEXCOORD0;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	//float4 vertexColor : COLOR; //Solid color (for debug)
};

//--------------------------------------------------------------------------------------
// normal mapping
//--------------------------------------------------------------------------------------
// This function returns the normal in world coordinates.
// The input struct contains tangent (t1), bitangent (t2) and normal (n) of the
// unperturbed surface in world coordinates. The perturbed normal in tangent space
// can be read from texNormalMap.
// The RGB values in this texture need to be normalized from (0, +1) to (-1, +1).
float3 calculateNormal(PSInput input)
{
    float3 normal = renderFlat ? normalize(cross(ddy(input.worldPosition.xyz), ddx(input.worldPosition.xyz))) : normalize(input.normal);
	
    if (hasNormalMap)
    {
        if (enableAutoTangent)
        {
            float3 localNormal = BiasX2(texNormalMap.Sample(surfaceSampler, input.textureCoordinates).xyz);
            normal = PeturbNormal(localNormal, input.worldPosition.xyz, normal, input.textureCoordinates);
        }
        else
        {
			// Normalize the per-pixel interpolated tangent-space
            float3 tangent = normalize(input.tangent);
            float3 biTangent = normalize(input.binormal);

			// Sample the texel in the bump map.
            float3 bumpMap = texNormalMap.Sample(surfaceSampler, input.textureCoordinates);
			// Expand the range of the normal value from (0, +1) to (-1, +1).
            bumpMap = mad(2.0f, bumpMap, -1.0f);
			// Calculate the normal from the data in the bump map.
            normal += mad(bumpMap.x, tangent, bumpMap.y * biTangent);
            normal = normalize(normal);
        }
    }
    
    return normal;
}


//--------------------------------------------------------------------------------------
// Blinn-Phong Lighting Reflection Model
//--------------------------------------------------------------------------------------
// Returns the sum of the diffuse and specular terms in the Blinn-Phong reflection model.
float4 calcBlinnPhongLighting(float4 LColor, float3 N, float4 diffuse, float3 L, float3 H, float4 specular, float shininess)
{
    //float4 Id = vMaterialTexture * diffuse * saturate(dot(N, L));
    //float4 Is = vMaterialSpecular * pow(saturate(dot(N, H)), sMaterialShininess);
    float4 f = lit(dot(N, L), dot(N, H), shininess);
    float4 Id = f.y * diffuse;
    float4 Is = min(f.z, diffuse.w) * specular;
    return (Id + Is) * LColor;
}

float4 lightSurface(float4 worldPosition, in float3 V, in float3 N, float4 diffuse, float4 specular, float shininess, float4 reflectColor)
{
    float4 acc_color = 0;
    // compute lighting
    for (int i = 0; i < lightCount; ++i)
    {
        if (lights[i].lightType == 1) // directional
        {
            float3 d = normalize((float3) lights[i].lightDirection); // light dir	
            float3 h = normalize(V + d);
            
            acc_color += calcBlinnPhongLighting(lights[i].lightColor, N, diffuse, d, h, specular, shininess);
        }
        else if (lights[i].lightType == 2)  // point
        {
            float3 d = (float3) (lights[i].lightPosition - worldPosition); // light dir
            float dl = length(d); // light distance
            
            if (lights[i].lightAttenuation.w < dl)
            {
                continue;
            }
            
            d = d / dl; // normalized light dir						
            float3 h = normalize(V + d); // half direction for specular
            float att = 1.0f / (lights[i].lightAttenuation.x + lights[i].lightAttenuation.y * dl + lights[i].lightAttenuation.z * dl * dl);
            acc_color = mad(att, calcBlinnPhongLighting(lights[i].lightColor, N, diffuse, d, h, specular, shininess), acc_color);
        }
        else if (lights[i].lightType == 3)  // spot
        {
            float3 d = (float3) (lights[i].lightPosition - worldPosition); // light dir
            float dl = length(d); // light distance
            
            if (lights[i].lightAttenuation.w < dl)
            {
                continue;
            }
            
            d = d / dl; // normalized light dir					
            float3 h = normalize(V + d); // half direction for specular
            float3 sd = normalize((float3) lights[i].lightDirection); // missuse the vLightDir variable for spot-dir

													/* --- this is the OpenGL 1.2 version (not so nice) --- */
													//float spot = (dot(-d, sd));
													//if(spot > cos(lightSpot[i].x))
													//	spot = pow( spot, lightSpot[i].y );
													//else
													//	spot = 0.0f;	
													/* --- */

													/* --- this is the  DirectX9 version (better) --- */
            float rho = dot(-d, sd);
            float spot = pow(saturate((rho - lights[i].lightSpot.x) / (lights[i].lightSpot.y - lights[i].lightSpot.x)), lights[i].lightSpot.z);
            float att = spot / (lights[i].lightAttenuation.x + lights[i].lightAttenuation.y * dl + lights[i].lightAttenuation.z * dl * dl);
            acc_color = mad(att, calcBlinnPhongLighting(lights[i].lightColor, N, diffuse, d, h, specular, shininess), acc_color);
        }
    }
    
    acc_color.a = diffuse.a;
    
    return saturate(acc_color);
}

//--------------------------------------------------------------------------------------
// PER PIXEL LIGHTING - BLINN-PHONG
//--------------------------------------------------------------------------------------
float4 mainPS(PSInput input) : SV_Target
{    
	//Renormalize interpolated vectors
    float3 N = calculateNormal(input);   

    //Get per pixel vector to eye-position
    float3 V = normalize(input.eye.xyz);

    //Add diffuse sampling
    float4 diffuse = materialDiffuse;
	
    if (hasDiffuseMap)
    {
        diffuse *= texDiffuseMap.Sample(surfaceSampler, input.textureCoordinates);
    }
	
    //diffuse = lerp(diffuse, input.vertexColor, vertexColorBlending);
	
    float alpha = 1;
	
    if (hasAlphaMap)
    {
        float4 color = texAlphaMap.Sample(surfaceSampler, input.textureCoordinates);
        alpha = color.a;
        diffuse.rgb *= color.rgb;
    }

    float4 specular = materialSpecular;
    float shininess = materialShininess;
    float4 reflectColor = materialReflect;
	
    if (hasSpecularMap)
    {
        specular *= texSpecularMap.Sample(surfaceSampler, input.textureCoordinates);
    }
	
    float4 color = lightSurface(input.worldPosition, V, N, diffuse, specular, shininess, reflectColor);
    float4 emissive = materialEmissive;
    float4 ambient = lightAmbient * materialAmbient;
	
    if (hasEmissiveMap)
    {
        emissive.rgb *= texEmissiveMap.Sample(surfaceSampler, input.textureCoordinates).rgb;
    }
	
    color.rgb += emissive.rgb + ambient.rgb;
    color.a = diffuse.a * alpha;
	
    return saturate(color);
}
