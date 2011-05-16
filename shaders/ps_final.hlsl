cbuffer frame: register(b0)
#include "cbuffer/frame"

Texture2D gbuffer0: register(t0);
Texture2D gbuffer1: register(t1);
Texture2D lbuffer: register(t2);

sampler sm_point : register(s0);

#include "code/uv_to_ray"
#include "struct/PPosition"


float4 main
(
	PPosition input
) : SV_Target0
{
	float2 uv = input.svposition.xy * rcpres;

	float3 normal = gbuffer0.Sample(sm_point, uv).xyz;
	float3 colour = gbuffer1.Sample(sm_point, uv).xyz;
	float mult = max(0.0, dot( view_i[2].xyz, normal));
	float4 ambient = 0*float4(mult * float3(0.02, 0.02, 0.02) * colour, 1.0);

	return (ambient + lbuffer.Sample(sm_point, uv)) / aperture;
}
