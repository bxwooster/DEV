cbuffer frame: register(b0)
#include "cbuffer/frame"

Texture2D gbuffer0: register(t0);
Texture2D gbuffer1: register(t1);
Texture2D zbuffer: register(t2);
Texture2D lbuffer: register(t3);

sampler sm_point : register(s0);

#include "code/uv_to_ray"


float4 ps_final
(
	float4 pos : SV_Position
) : SV_Target0
{
	float2 uv = pos.xy * rcpres;

	if (zbuffer.Sample(sm_point, uv).x == 1.0) //sky
	{
		float4 horizon = float4(0.6, 0.75, 0.9, 1.0);
		float4 zenith = float4(0.25, 0.35, 0.9, 1.0);

		float3 world_ray = mul( (float3x3)view_i, float3(uv_to_ray(uv), 1.0) );
		return lerp(horizon, zenith, -world_ray.z );
	}
	
	float3 normal = gbuffer0.Sample(sm_point, uv).xyz;
	float3 colour = gbuffer1.Sample(sm_point, uv).xyz;
	float mult = max(0.0, dot( view_i[2].xyz, normal));
	float4 ambient = float4(mult * float3(0.02, 0.02, 0.02) * colour, 1.0);

	return (ambient + lbuffer.Sample(sm_point, uv)) / aperture;
}
