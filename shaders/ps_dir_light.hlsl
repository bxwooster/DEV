cbuffer frame: register(b0)
#include "cbuffer/frame"
cbuffer light: register(b4)
#include "cbuffer/light"

Texture2D gbuffer0: register(t0);
Texture2D gbuffer1: register(t1);
Texture2D zbuffer: register(t2);
Texture2D shadowmap: register(t3);

sampler sm_point : register(s0);
#include "code/uv_to_ray"


float4 ps_dir_light
(
	float2 uv : Position,
	float4 pos : SV_Position
) : SV_Target0
{
	float light_scale = 200.0;
	float z_neg = -z_near / (1.0 - zbuffer.Sample(sm_point, uv).x);
	float4 surface_pos = float4( uv_to_ray(uv) * z_neg, z_neg, 1.0 );
	float4 reprojected = mul(reproject, surface_pos);
	float in_front = reprojected.w > 0;
	reprojected /= reprojected.w;
	float inside_cone = length(reprojected.xy) < 1;
	clip(in_front * inside_cone - 1);
	float2 s_uv = float2(reprojected.x, -reprojected.y) * 0.5 + 0.5;
	float4 s = z_near / (1.0 - shadowmap.Gather(sm_point, s_uv));

	float3 lightvec = light_pos - surface_pos.xyz;
	float l = length(lightvec);
	float p = dot(light_matrix[2].xyz, lightvec);
	float lighted = dot(p - 0.2 <= s, 0.25);

	float3 normal = gbuffer0.Sample(sm_point, uv).xyz;
	float3 colour = gbuffer1.Sample(sm_point, uv).xyz;

    float radiance = lighted * max(0.0, dot( lightvec, normal )) / (l * l * l) * light_scale;

	return float4(radiance * light_colour * colour, 1.0);
}
