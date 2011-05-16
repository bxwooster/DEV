cbuffer frame: register(b0)
#include "cbuffer/frame"
cbuffer frame: register(b1)
#include "cbuffer/object"

#include "struct/PPositionNormal"
#include "struct/OITFragment"
#include "code/u8x2_f16"
#include "code/u8x4"
#include "code/normal"

RWByteAddressBuffer start_buffer : register(u2);
RWStructuredBuffer<OITFragment> fragment_buffer : register(u3);


void main
(
	PPositionNormal input,
out float4 g0 : SV_Target0,
out float4 g1 : SV_Target1
){
	float4 colour = __colour; //{ 1, 1, 1, 1 };
	float specular = 1;
	float2 enc_normal = normal_encode(normalize(input.normal));

	if (colour.a == 1.0)
	{
		g0 = float4(enc_normal, specular, 0);
		g1 = colour;
		return;
	}
	else if (colour.a == 0.0) discard;
	else
	{
		uint old_index;
		uint index = fragment_buffer.IncrementCounter();

		//uint dim, stride; fragment_buffer.GetDimensions(dim, stride); //!
		uint dim = 2 * res.x * res.y;
		if (index >=  dim) discard; // overflow

		uint start = 4 * (input.svposition.y * res.x + input.svposition.x + 2);
		start_buffer.InterlockedExchange(start, index, old_index);
		
		OITFragment fragment;
		float depth = input.svposition.z;
		fragment.normal16_depth16 = u8x2_f16_pack(float3(enc_normal, depth));
		fragment.colour = u8x4_pack(colour);
		fragment.spec8_next24 = (uint(specular * 255 + 0.5) << 24) | old_index;
		fragment_buffer[index] = fragment;

		discard;
	}
}