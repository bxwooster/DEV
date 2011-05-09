cbuffer object_z: register(b0)
#include "cbuffer/object"

#include "struct/Empty"
#include "struct/Pixel"

[maxvertexcount(12)]
void main
(
	point Empty empty[1],
	inout TriangleStream<Pixel> stream
){
	Pixel pixel;

	float4 vertices[] =
	{
		{ 0, 0, 0, 1 },
		{ 1,-1, 0, 0 },
		{ 1, 1, 0, 0 },

		{ 0, 0, 0, 1 },
		{ 1, 1, 0, 0 }, 
		{-1, 1, 0, 0 },

		{ 0, 0, 0, 1 },
		{-1, 1, 0, 0 },
		{-1,-1, 0, 0 },

		{ 0, 0, 0, 1 },
		{-1,-1, 0, 0 }, 
		{ 1,-1, 0, 0 },
	};

	for (uint id = 0; id < 12; id++)
	{
		for (uint v = 0; v < 3; v++)
		{
			float4 vertex = vertices[ 3 * id + v ];
			pixel.pos = mul(world_view_proj, vertex);
			pixel.normal = normalize( mul( (float3x3)world_view, float3(0, 0, 1 ) ));
			stream.Append(pixel);
		}
		stream.RestartStrip();
	}
}
