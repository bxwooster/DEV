cbuffer object_cube_z: register(b0)
#include "cbuffer/object_cube_z"

#include "struct/VPosition"

void main
(
	VPosition input,
out VPosition output
){
	output = input;
}
