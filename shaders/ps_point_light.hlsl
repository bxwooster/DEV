cbuffer frame: register(b0)
#include "cbuffer/frame"
cbuffer light: register(b1)
#include "cbuffer/light"

Texture2D gbuffer0: register(t0);
Texture2D gbuffer1: register(t1);
Texture2D zbuffer: register(t2);
TextureCube shadowcube: register(t3);

sampler sm_point : register(s0);

#include "code/uv_to_ray"


float4 ps_point_light
(
	float2 uv : Position,
	float4 pos : SV_Position
) : SV_Target0
{
	float light_scale = 200.0;
	float z_neg = -z_near / (1.0 - zbuffer.Sample(sm_point, uv).x);
	float4 surface_pos = float4( uv_to_ray(uv) * z_neg, z_neg, 1.0 );
	float4 reprojected = mul(reproject, surface_pos);

	float4 s = z_near / (1.0 - shadowcube.Gather(sm_point, reprojected.xyz));

	float3 lightvec = light_pos - surface_pos.xyz;
	float l = length(lightvec);
	float3 p = abs(mul(light_matrix, float4(lightvec, 0)).xyz);
	float m = max(p.x, max(p.y, p.z));
	float lighted = dot(m - 0.2 <= s, 0.25);

	float3 normal = gbuffer0.Sample(sm_point, uv).xyz;
	float3 colour = gbuffer1.Sample(sm_point, uv).xyz;

    float radiance = lighted * max(0.0, dot( lightvec, normal )) / (l * l * l) * light_scale;

	return float4(radiance * light_colour * colour, 1.0);
}
