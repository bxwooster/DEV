cbuffer object_z: register(b0)
#include "cbuffer/light"

#include "struct/Empty"
#include "struct/ScreenPixel"

[maxvertexcount(18)]
void gs_dir_light
(
	point Empty empty[1],
	inout TriangleStream<ScreenPixel> stream
){
	ScreenPixel pixel;

	float3 vertices[5] =
	{
		{ 0, 0, 0 },
		{ 0, 1, 1 },
		{ 1, 0, 1 },
		{ 0,-1, 1 },
		{-1, 0, 1 }
	};

	uint3 indices[6] =
	{
		{ 0, 2, 1 },
		{ 0, 3, 2 },
		{ 0, 4, 3 },
		{ 0, 1, 4 },
		{ 1, 2, 3 },
		{ 1, 3, 4 }
	};

	for (uint id = 0; id < 6; id++)
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
