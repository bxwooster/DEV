cbuffer frame: register(b0)
#include "cbuffer/frame"
cbuffer light: register(b1)
#include "cbuffer/light"

Texture2D<uint2> gbuffer: register(t0);
Texture2D<float> zbuffer: register(t1);
Texture2D<float> shadowmap: register(t2);

#include "struct/OITFragment"

RWByteAddressBuffer start_buffer : register(u1);
RWStructuredBuffer<OITFragment> consolidated_buffer : register(u2);

sampler sm_point : register(s0);

#include "struct/PPosition"
#include "code/uv_to_ray"

float shadowmap_test(float4 reprojected, float3 lightvec)
{
	float in_front = reprojected.w > 0;
	reprojected /= reprojected.w;
	float inside_cone = length(reprojected.xy) < 1;
	if (in_front * inside_cone == 0.0) return 0;

	float2 s_uv = float2(reprojected.x, -reprojected.y) * 0.5 + 0.5;
	float4 s = z_near / (1.0 - shadowmap.Gather(sm_point, s_uv));
	float m = dot(viewI_light_view[2].xyz, lightvec);
	return dot(m <= s, 0.25);
}

#define SHADOW_TEST shadowmap_test
#include "code/common_light"

float4 main
(
	PPosition input
) : SV_Target0
{
	return common_light(input);
}
