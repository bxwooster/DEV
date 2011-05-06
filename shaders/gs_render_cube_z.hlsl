cbuffer object_cube_z: register(b0)
#include "cbuffer/object_cube_z"

#include "struct/GSInput"
#include "struct/GSOutput"


[maxvertexcount(18)]
void gs_render_cube_z
(
	triangle GSInput input[3],
	inout TriangleStream<GSOutput> stream
){
	GSOutput output;
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