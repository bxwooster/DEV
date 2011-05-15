cbuffer frame: register(b0)
#include "cbuffer/frame"
cbuffer frame: register(b1)
#include "cbuffer/object"

#include "struct/PPositionNormal"
#include "struct/OITFragment"
#include "code/f16x2"
#include "code/u8x4"

RWByteAddressBuffer start_buffer : register(u2);
RWStructuredBuffer<OITFragment> fragment_buffer : register(u3);


void main
(
	PPositionNormal input,
out float4 g0 : SV_Target0,
out float4 g1 : SV_Target1
){
	float4 colour = __colour;//{ 1, 1, 1, 1 };
	
	if (colour.a == 0.0) discard;
	if (colour.a == 1.0)
	{
		g0.xyz = input.normal;
		g0.w = 0.0;
		g1 = colour;
		return;
	}
	else
	{
		uint old_index;
		uint index = fragment_buffer.IncrementCounter();
		if (index > 2 * res.x * res.y) discard; //overflow //!

		uint start = 4 * (input.svposition.y * res.x + input.svposition.x);
		start_buffer.InterlockedExchange(start, index, old_index);
		
		OITFragment fragment;
		float2 packed = float2(input.normal.z, input.svposition.z);
		fragment.normal_xy = f16x2_pack(input.normal.xy);
		fragment.normal_z_and_depth = f16x2_pack(packed);
		fragment.colour_rgba = u8x4_pack(colour);
		fragment.next = old_index;
		fragment_buffer[index] = fragment;

		discard;
	}
}