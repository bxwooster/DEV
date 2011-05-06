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
	for (uint id = 0; id < 3; id++)
	{
		if (id == 0) position = float2(0, 0);
		if (id == 1) position = float2(2, 0);
		if (id == 2) position = float2(0, 2);

		pixel.pos = float4(position * 2.0 - 1.0, 0.0, 1.0);
		pixel.uv = float2(position.x, 1.0 - position.y);

		stream.Append(pixel);
	}
}
