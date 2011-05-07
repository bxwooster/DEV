cbuffer object_cube_z: register(b0)
#include "cbuffer/object_cube_z"

struct Input
{
	float4 position : POSITION;
};

struct Output
{
	float4 position : SV_Position;
	uint index : SV_RenderTargetArrayIndex;
};


[maxvertexcount(18)]
void gs_render_cube_z
(
	triangle Input input[3],
	inout TriangleStream<Output> stream
){
	Output output;
	for (int i = 0; i < 6; i++)
	{
		output.index = i;
		for (int v = 0; v < 3; v++)
		{
			output.position = mul(cubeproj[i], input[v].position);

			stream.Append( output );
		}
		stream.RestartStrip();
	}
}