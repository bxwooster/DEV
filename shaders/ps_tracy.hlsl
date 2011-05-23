cbuffer frame: register(b0)
#include "cbuffer/frame"
cbuffer tracy: register(b1)
#include "cbuffer/tracy"

#include "struct/PPosition"
#include "code/uv_to_ray"
#include "code/normal"
#include "code/u16x2"
#include "code/u8x4"


float dist(float3 world)
{
	world -= float3(0,0,1);
	float torus = length(float2(length(world.xy) - 1.0, world.z)) - 0.25;
	float cube = max(max(abs(world.x), abs(world.y)), abs(world.z)) - 1.0;
	float sphere = length(world) - sqrt(2.0);
	float spheres = (length(frac(world + 0.5) - 0.5) - 0.25);

	return lerp(cube, spheres, interp);
}

void main
(
	PPosition input,
out uint2 output : SV_Target0,
out float depth : SV_Depth
){
	float2 uv = input.svposition.xy * rcpres;

	float4 pos = float4(eye, 0.0);
	float4 dir = mul( viewI, -normalize(float4(uv_to_ray(uv), 1, 0)));
	float4 cdir = mul( viewI, -normalize(float4(0, 0, 1, 0)));
	float4 ray = float4(dir.xyz, 1.0);

	float d = 1.0;
	for(int i = 0; i < steps && d / pos.w > threshold; i++)
	{
		d = dist(pos.xyz);
		pos += ray * d;
	}
	clip(steps - i - 1);

	d = dist(pos.xyz);
	float3 normal;
	normal.x = dist(pos.xyz + float3(eps, 0, 0)) - d;
	normal.y = dist(pos.xyz + float3(0, eps, 0)) - d;
	normal.z = dist(pos.xyz + float3(0, 0, eps)) - d;
	normal = normalize( mul((float3x3)view, normal) );

	output.x = u16x2_pack(normal_encode(normal));
	output.y = u8x4_pack(1);

	depth = 1.0 - z_near / (pos.w * dot(dir, cdir)); //!
}
