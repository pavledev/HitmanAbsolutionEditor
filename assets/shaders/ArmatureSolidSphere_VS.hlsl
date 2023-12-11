#include "Common.hlsl"

// Sphere radius
static const float rad = 0.05;

struct VS_INPUT
{
    float2 pos : POSITION;
};

struct VS_OUTPUT
{
	float3 viewPosition : POSITION;
	float4 worldPos : SV_POSITION;
	//matrix sphereMatrix : ATTRIBUTE0;
};

matrix inverse(matrix m)
{
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    matrix ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}

VS_OUTPUT mainVS(const VS_INPUT input)
{
	VS_OUTPUT output;
	
    //sphereMatrix = inverse(modelView);

    bool is_persp = projection[3][3] == 0.f;

    /* This is the local space camera ray (not normalize).
     * In perspective mode it's also the viewspace position
     * of the sphere center. */
    float3 cam_ray = is_persp ? modelView[3].xyz : float3(0.0, 0.0, -1.0);
    cam_ray = mul((float3x3)sphereMatrix, cam_ray);

    /* Sphere center distance from the camera (persp) in local space. */
    float cam_dist = length(cam_ray);

    /* Compute view aligned orthonormal space. */
    float3 z_axis = cam_ray / cam_dist;
    float3 x_axis = normalize(cross(sphereMatrix[1].xyz, z_axis));
    float3 y_axis = cross(z_axis, x_axis);

    float z_ofs = -rad - 1e-8; /* offset to the front of the sphere */
    if (is_persp)
    {
        /* For perspective, the projected sphere radius
         * can be bigger than the center disc. Compute the
         * max angular size and compensate by sliding the disc
         * towards the camera and scale it accordingly. */
        const float half_pi = 3.1415926 * 0.5;
        /* Let be (in local space):
         * V the view vector origin.
         * O the sphere origin.
         * T the point on the target circle.
         * We compute the angle between (OV) and (OT). */
        float a = half_pi - asin(rad / cam_dist);
        float cos_b = cos(a);
        float sin_b = sqrt(clamp(1.0 - cos_b * cos_b, 0.0, 1.0));
#if 1
        /* Instead of choosing the biggest circle in screenspace,
         * we choose the nearest with the same angular size. This
         * permit us to leverage GL_ARB_conservative_depth in the
         * fragment shader. */
        float minor = cam_dist - rad;
        float major = cam_dist - cos_b * rad;
        float fac = minor / major;
        sin_b *= fac;
#else
        z_ofs = -rad * cos_b;
#endif
        x_axis *= sin_b;
        y_axis *= sin_b;
    }

    /* Camera oriented position (but still in local space) */
    float3 cam_pos = x_axis * input.pos.x + y_axis * input.pos.y + z_axis * z_ofs;

    float4 pos_4d = float4(cam_pos, 1.0);
    float4 V = mul(modelView, pos_4d);
    output.worldPos = mul(projection, V);
    output.viewPosition = V.xyz;

    //finalStateColor = state_color.xyz;
    //finalBoneColor = bone_color.xyz;
	
	return output;
}