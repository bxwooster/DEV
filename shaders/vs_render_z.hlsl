cbuffer object_z: register(b0)
#include "cbuffer/object_z"

#include "struct/VPosition"
#include "struct/PPosition"


void main
(
	VPosition input,
out PPosition output
){
	output.svposition = mul( world_view_proj, float4(input.position, 1.0) );
}
