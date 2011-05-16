cbuffer frame: register(b0)
#include "cbuffer/frame"
cbuffer light: register(b1)
#include "cbuffer/light"

Texture2D gbuffer0: register(t0);
Texture2D gbuffer1: register(t1);
Texture2D zbuffer: register(t2);
TextureCube shadowcube: register(t3);

#include "struct/OITFragment"

RWByteAddressBuffer start_buffer : register(u1);
RWStructuredBuffer<OITFragment> scattered_buffer : register(u2);

sampler sm_point : register(s0);

#include "struct/PPosition"
#include "code/uv_to_ray"

float shadowcube_test(float4 reprojected, float3 lightvec)
{
	float4 s = z_near / (1.0 - shadowcube.Gather(sm_point, reprojected.xyz));
	float3 p = abs(mul(viewI_light_view, float4(lightvec, 0)).xyz);
	float m = max(p.x, max(p.y, p.z));
	return dot(m <= s, 0.25);
}

#define SHADOW_TEST shadowcube_test(reprojected, lightvec)
#include "code/common_light"

float4 main
(
	PPosition input
) : SV_Target0
{
	return common_light(input);
}
