cbuffer frame: register(b0)
#include "cbuffer/frame"

Texture2D<uint2> gbuffer: register(t0);
Texture2D<float4> lbuffer: register(t1);

sampler sm_point : register(s0);

#include "code/uv_to_ray"
#include "code/normal"
#include "code/u16x2"
#include "code/u8x4"
#include "struct/PPosition"


float4 main
(
	PPosition input
) : SV_Target0
{
	float2 uv = input.svposition.xy * rcpres;
	int3 iuv = int3(input.svposition.xy, 0);

	uint2 packed = gbuffer.Load(iuv);
	float3 normal = normal_decode(u16x2_unpack(packed.x));
	float3 colour = u8x4_unpack(packed.y).xyz;
	float mult = max(0.0, dot( view_i[2].xyz, normal));
	float4 ambient = 0*float4(mult * float3(0.02, 0.02, 0.02) * colour, 1.0);

	float4 final = (ambient + lbuffer.Sample(sm_point, uv)) / aperture;
	float4 reinhard = final / (1 + final);

	float4 x = max(0, final - 0.004);
	float4 jim_richard = (x*(6.2*x+.5))/(x*(6.2*x+1.7)+0.06);

	return jim_richard;
}
