cbuffer frame: register(b0)
#include "cbuffer/frame"

#include "struct/OITFragment"
#include "code/sort4"

Texture2D zbuffer: register(t0);
sampler sm_point : register(s0);

RWByteAddressBuffer start_buffer : register(u0);
RWStructuredBuffer<OITFragment> scattered_buffer : register(u1);
RWStructuredBuffer<OITFragment> consolidated_buffer : register(u2);


// Wavefront <= 64
[numthreads(8, 8, 1)]
void main(uint2 position : SV_DispatchThreadID)
{
	uint start = 4 * (position.y * res.x + position.x + 2);
	uint index = start_buffer.Load(start);
	if (index == 0) return;

	float solid_depth = zbuffer[position].x;
	float4 depths = 1;
	OITFragment fragments[4];

	[unroll]
	for (uint i = 0; i < 4 && index != 0; i++)
	{
		fragments[i] = scattered_buffer[index];
		uint packed = fragments[i].normal16_depth16;
		depths[i] = f16tof32(packed >> 16);
		index = fragments[i].spec8_next24 & 0x00ffffff;
	}

	uint4 indices = uint4(0, 1, 2, 3);
	sort4(depths, indices);

	[unroll]
	for (uint N = 0; N < 4; N++)
		if (depths[N] < solid_depth) break;

	start_buffer.InterlockedAdd(0, N, index);
	start_buffer.Store(start, index | (N << 24));

	[unroll]
	for (uint j = 0; j < 4 && j < N; j++)
	{
		consolidated_buffer[index + j] = fragments[indices[j]];
	}
}
