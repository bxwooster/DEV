#include "struct/ScreenPixel"
#include "struct/Empty"


[maxvertexcount(3)]
void gs_fullscreen
(
	point Empty empty[1],
	inout TriangleStream<ScreenPixel> stream
){
	ScreenPixel pixel;
	float2 position;
	float2 positions[] = { {0, 0}, {2, 0}, {0, 2} };

	for (uint id = 0; id < 3; id++)
	{
		position = positions[id];
		pixel.pos = float4(position * 2 - 1, 0, 1);
		stream.Append(pixel);
	}
}
