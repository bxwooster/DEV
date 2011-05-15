#include "struct/Empty"
#include "struct/PPosition"


[maxvertexcount(3)]
void main
(
	point Empty empty[1],
	inout TriangleStream<PPosition> stream
){
	PPosition output;
	float2 positions[] = { {0, 0}, {0, 2}, {2, 0} };

	for (uint id = 0; id < 3; id++)
	{
		output.svposition = float4(positions[id] * 2 - 1, 0, 1);
		stream.Append(output);
	}
}
