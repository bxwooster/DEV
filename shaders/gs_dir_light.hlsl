cbuffer object_z: register(b0)
#include "cbuffer/object_z"

#include "struct/Empty"

struct Output
{
	float4 pos : SV_Position;
};


[maxvertexcount(18)]
void gs_dir_light
(
	point Empty empty[1],
	inout TriangleStream<Output> stream
){
	Output output;

	float3 vertices[5] =
	{
		{ 0, 0, 0 },
		{ 0, 1, 1 },
		{ 0,-1, 1 },
		{ 1, 0, 1 },
		{-1, 0, 1 }
	};

	uint3 indices[6] =
	{
		{ 0, 1, 2 },
		{ 0, 2, 3 },
		{ 0, 3, 4 },
		{ 0, 4, 1 },
		{ 1, 2, 3 },
		{ 2, 3, 4 }
	};

	for (uint id = 0; id < 6; id++)
	{
		for (uint v = 0; id < 3; id++)
		{
			float4 vertex = float4(vertices[ indices[id][v] ], 1.0);
			output.pos = mul(world_view_proj, vertex);
			stream.Append(output);
		}
		stream.RestartStrip();
	}
}
