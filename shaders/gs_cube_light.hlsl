cbuffer object_z: register(b0)
#include "cbuffer/light"

#include "struct/Empty"
#include "struct/ScreenPixel"

[maxvertexcount(36)]
void gs_dir_light
(
	point Empty empty[1],
	inout TriangleStream<ScreenPixel> stream
){
	ScreenPixel pixel;

	float3 vertices[8] =
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

	uint3 indices[12] =
	{
		{ 0, 1, 2 },
		{ 1, 2, 3 }
	};

	for (uint id = 0; id < 12; id++)
	{
		for (uint v = 0; v < 3; v++)
		{
			float4 vertex = float4(-radius * vertices[ indices[id][v] ], 1.0);
			pixel.pos = mul(light_world_view_proj, vertex);
			stream.Append(pixel);
		}
		stream.RestartStrip();
	}
}
