cbuffer object_z: register(b0)
#include "cbuffer/light"

#include "struct/Empty"
#include "struct/PPosition"


[maxvertexcount(36)]
void main
(
	point Empty empty[1],
	inout TriangleStream<PPosition> stream
){
	PPosition output;

	float3 vertices[] =
	{
		{-1,-1,-1 },
		{-1,-1, 1 },
		{-1, 1,-1 },
		{-1, 1, 1 },
		{ 1,-1,-1 },
		{ 1,-1, 1 },
		{ 1, 1,-1 },
		{ 1, 1, 1 }
	};

	uint3 indices[] =
	{
		{ 0, 1, 2 },
		{ 1, 3, 2 },
		{ 4, 6, 5 },
		{ 5, 6, 7 },

		{ 0, 4, 1 },
		{ 4, 5, 1 },
		{ 2, 3, 6 },
		{ 3, 7, 6 },

		{ 1, 5, 3 },
		{ 5, 7, 3 },
		{ 0, 2, 4 },
		{ 4, 2, 6 }
	};

	for (uint id = 0; id < 12; id++)
	{
		for (uint v = 0; v < 3; v++)
		{
			float4 vertex = float4(-radius * vertices[ indices[id][v] ], 1.0);
			output.svposition = mul(light_world_view_proj, vertex);
			stream.Append(output);
		}
		stream.RestartStrip();
	}
}
