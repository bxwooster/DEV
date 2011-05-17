cbuffer frame: register(b0)
#include "cbuffer/frame"

sampler sm_point : register(s0);

#include "code/uv_to_ray"
#include "struct/PPosition"


float3 main
(
	PPosition input
) : SV_Target0
{
	float2 uv = input.svposition.xy * rcpres;

	float4 horizon = float4(0.6, 0.75, 0.9, 1.0);
	float4 zenith = float4(0.25, 0.35, 0.9, 1.0);

	float3 world_ray = mul( (float3x3)view_i, float3(uv_to_ray(uv), 1.0) );
	return lerp(horizon, zenith, -world_ray.z);
}
