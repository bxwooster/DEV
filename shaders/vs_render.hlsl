cbuffer object: register(b0)
#include "cbuffer/object"

#include "struct/Vertex"
#include "struct/Pixel"


void main
(
	Vertex vertex,
	out Pixel pixel
){
	pixel.pos = mul( world_view_proj, float4(vertex.position, 1.0) );
	pixel.normal = normalize( mul( (float3x3)world_view, vertex.normal ) );
}
