cbuffer object_z: register(b2)
#include "cbuffer/object_z"

#include "struct/Vertex"


void vs_render_z
(
	Vertex vertex,
	out float4 position : SV_Position
){
	position = mul( world_lightview_lightproj, vertex.position );
}
