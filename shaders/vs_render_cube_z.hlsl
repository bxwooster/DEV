cbuffer object_cube_z: register(b3)
#include "cbuffer/object_cube_z"

#include "struct/Vertex"
#include "struct/Pixel"


void vs_render_cube_z
(
	Vertex vertex,
	out float4 position : POSITION
){
	position = vertex.position;
}
