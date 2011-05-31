cbuffer frame: register(b0)
#include "cbuffer/frame"
cbuffer frame: register(b1)
#include "cbuffer/object"

#include "struct/PPositionNormal"
#include "struct/OITFragment"
#include "code/u16x2"
#include "code/u8x4"
#include "code/u8_u24"
#include "code/normal"

RWByteAddressBuffer start_buffer : register(u1);
RWStructuredBuffer<OITFragment> scattered_buffer : register(u2);


void main
(
	PPositionNormal input,
out uint2 output : SV_Target0
){
	float4 colour = __colour; //{ 1, 1, 1, 1 };
	colour.a = 1;
	float specular = 1;
	float2 encoded_normal = normal_encode(normalize(input.normal));
	// Interpolation means we have to renormalize for encoding.
	// Update: Not sure if correct.

	if (colour.a == 1.0)
	{
		output.x = u16x2_pack(encoded_normal);
		output.y = u8x4_pack(float4(colour.xyz, specular));
		return;
	}
	else if (colour.a == 0.0) discard;
	else
	{
		uint old_index;
		uint index = scattered_buffer.IncrementCounter();

		//uint dim, stride; scattered_buffer.GetDimensions(dim, stride); //!
		uint dim = 2 * res.x * res.y;
		if (index >=  dim) discard; // overflow

		uint start = 4 * (input.svposition.y * res.x + input.svposition.x + 2);
		start_buffer.InterlockedExchange(start, index, old_index);

		OITFragment fragment;
		float depth = input.svposition.z;
		fragment.normal = u16x2_pack(encoded_normal);
		fragment.colour = u8x4_pack(float4(colour.xyz, specular));
		fragment.alpha8_depth24 = u8_u24_pack(float2(colour.a, depth));
		fragment.next = old_index;
		scattered_buffer[index] = fragment;

		discard;
	}
}