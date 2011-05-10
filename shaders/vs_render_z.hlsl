cbuffer object_z: register(b0)
#include "cbuffer/object_z"


void main
(
	float3 position : POSITION,
	out float4 output : SV_Position
){
	output = mul( world_view_proj, float4(position, 1.0) );
}
