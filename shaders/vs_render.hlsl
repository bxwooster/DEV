cbuffer object: register(b0)
#include "cbuffer/object"

#include "struct/VPositionNormal"
#include "struct/PPositionNormal"

void main
(
	VPositionNormal input,
out PPositionNormal output
){
	output.svposition = mul( world_view_proj, float4(input.position, 1.0) );
	output.normal = normalize( mul( (float3x3)world_view, input.normal ) );
}
