cbuffer object_cube_z: register(b0)
#include "cbuffer/object_cube_z"

#include "struct/VPosition"
#include "struct/PPosition"

struct Output
{
	PPosition p;
	uint index : SV_RenderTargetArrayIndex;
};


[maxvertexcount(18)]
void main
(
	triangle VPosition input[3],
	inout TriangleStream<Output> stream
){
	Output output;
	for (int i = 0; i < 6; i++)
	{
		output.index = i;
		for (int v = 0; v < 3; v++)
		{
			output.p.svposition = mul(cubeproj[i], float4(input[v].position, 1.0));

			stream.Append( output );
		}
		stream.RestartStrip();
	}
}