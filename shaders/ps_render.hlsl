#include "struct/Pixel"


void main
(
	Pixel pixel,
	out float4 g0 : SV_Target0,
	out float4 g1 : SV_Target1
){
	g0.xyz = pixel.normal;
	g1.xyz = 1.0; // some colour
}
