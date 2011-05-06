cbuffer object: register(b0)
#include "cbuffer/object"

#include "struct/Vertex"
#include "struct/Pixel"


void vs_render
(
	Vertex vertex,
	out Pixel pixel
){
	pixel.pos = mul( world_view_proj, vertex.position );
	pixel.normal = normalize( mul( (float3x3)world_view, vertex.normal ) );
}
