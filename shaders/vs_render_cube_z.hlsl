cbuffer object_cube_z: register(b0)
#include "cbuffer/object_cube_z"


void main
(
	float3 position : POSITION,
	out float3 output : POSITION
){
	output = position;
}
