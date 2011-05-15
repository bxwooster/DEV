cbuffer object_z: register(b0)
#include "cbuffer/light"

#include "struct/Empty"
#include "struct/PPosition"


[maxvertexcount(18)]
void main
(
	point Empty empty[1],
	inout TriangleStream<PPosition> stream
){
	PPosition output;

	float3 vertices[] =
	{
		{ 0, 0, 0 },
		{ 1, 1, 1 },
		{ 1,-1, 1 },
		{-1,-1, 1 },
		{-1, 1, 1 }
	};

	uint3 indices[] =
	{
		{ 0, 1, 2 },
		{ 0, 2, 3 },
		{ 0, 3, 4 },
		{ 0, 4, 1 },
		{ 1, 3, 2 },
		{ 1, 4, 3 }
	};

	for (uint id = 0; id < 6; id++)
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
