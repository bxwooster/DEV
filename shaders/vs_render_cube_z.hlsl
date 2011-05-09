cbuffer object_cube_z: register(b0)
#include "cbuffer/object_cube_z"

#include "struct/Vertex"
#include "struct/Pixel"


void main
(
	Vertex vertex,
	out float3 position : POSITION
){
	position = vertex.position;
}
