static const float bias = 0.2;
static const float light_scale = 200.0;
static const float eps = 3e-7;


cbuffer frame: register(b0)
#include "cbuffer/frame"

cbuffer object: register(b1)
#include "cbuffer/object"

cbuffer object_z: register(b2)
#include "cbuffer/object_z"

cbuffer object_cube_z: register(b3)
#include "cbuffer/object_cube_z"

cbuffer light: register(b4)
#include "cbuffer/light"


Texture2D gbuffer0: register(t0);
Texture2D gbuffer1: register(t1);
Texture2D zbuffer: register(t2);
Texture2D shadowmap: register(t3);
TextureCube shadowcube: register(t4);
Texture2D lbuffer: register(t5);


sampler sm_point : register(s0);


struct Vertex
{
	float4 position : POSITION;
	float3 normal : NORMAL;
};

struct Pixel
{
	float4 pos : SV_Position;
	float3 normal : Normal;
};

struct ScreenPixel
{
	float2 uv : Position;
	float4 pos : SV_Position;
};

struct GSInput
{
	float4 position : POSITION;
};

struct Empty
{
	float nothing : POSITION;
};

struct GSOutput
{
	float4 position : SV_Position;
	uint index : SV_RenderTargetArrayIndex;
};

float2 uv_to_ray(float2 uv)
{
	return (2 * uv - 1) * xy_to_ray;
}

void vs_render( Vertex vertex, out Pixel pixel )
{
	pixel.pos = mul( world_view_proj, vertex.position );
	pixel.normal = normalize( mul( (float3x3)world_view, vertex.normal ) );
}

[maxvertexcount(18)]
void gs_render_cube_z( triangle GSInput input[3], inout TriangleStream<GSOutput> stream)
{
	GSOutput output;
	for (int i = 0; i < 6; i++)
	{
		output.index = i;
		for (int v = 0; v < 3; v++)
		{
			output.position = mul(cubeproj[i], input[v].position);

			stream.Append( output );
		}
		stream.RestartStrip();
	}
}

void vs_render_cube_z( Vertex vertex, out float4 position : POSITION )
{
	position = vertex.position;
}

void vs_render_z( Vertex vertex, out float4 position : SV_Position )
{
	position = mul( world_lightview_lightproj, vertex.position );
}
		
void ps_render( Pixel pixel,
	out float4 g0 : SV_Target0,
	out float4 g1 : SV_Target1 )
{
	g0.xyz = pixel.normal;
	g1.xyz = 1.0; // some colour
}

void vs_noop( out Empty empty )
{

}

[maxvertexcount(3)]
void gs_fullscreen(point Empty empty[1], inout TriangleStream<ScreenPixel> stream)
{
	ScreenPixel pixel;
	float2 position;
	for (uint id = 0; id < 3; id++)
	{
		if (id == 0) position = float2(0, 0);
		if (id == 1) position = float2(2, 0);
		if (id == 2) position = float2(0, 2);

		pixel.pos = float4(position * 2.0 - 1.0, 0.0, 1.0);
		pixel.uv = float2(position.x, 1.0 - position.y);

		stream.Append(pixel);
	}
}

float4 ps_dir_light(float2 uv : Position, float4 pos : SV_Position) : SV_Target0
{
	float z_neg = -z_near / (1.0 - zbuffer.Sample(sm_point, uv).x);
	float4 surface_pos = float4( uv_to_ray(uv) * z_neg, z_neg, 1.0 );
	float4 reprojected = mul(reproject, surface_pos);
	float in_front = reprojected.w > 0;
	reprojected /= reprojected.w;
	float inside_cone = length(reprojected.xy) < 1;
	clip(in_front * inside_cone - 1);
	float2 s_uv = float2(reprojected.x, -reprojected.y) * 0.5 + 0.5;
	float4 s = z_near / (1.0 - shadowmap.Gather(sm_point, s_uv));

	float3 lightvec = light_pos - surface_pos.xyz;
	float l = length(lightvec);
	float p = dot(light_matrix[2].xyz, lightvec);
	float lighted = dot(p - bias <= s, 0.25);

	float3 normal = gbuffer0.Sample(sm_point, uv).xyz;
	float3 colour = gbuffer1.Sample(sm_point, uv).xyz;

    float radiance = lighted * max(0.0, dot( lightvec, normal )) / (l * l * l) * light_scale;

	return float4(radiance * light_colour * colour, 1.0);
}

float4 ps_point_light(float2 uv : Position, float4 pos : SV_Position) : SV_Target0
{
	float z_neg = -z_near / (1.0 - zbuffer.Sample(sm_point, uv).x);
	float4 surface_pos = float4( uv_to_ray(uv) * z_neg, z_neg, 1.0 );
	float4 reprojected = mul(reproject, surface_pos);

	float4 s = z_near / (1.0 - shadowcube.Gather(sm_point, reprojected.xyz));

	float3 lightvec = light_pos - surface_pos.xyz;
	float l = length(lightvec);
	float3 p = abs(mul(light_matrix, float4(lightvec, 0)).xyz);
	float m = max(p.x, max(p.y, p.z));
	float lighted = dot(m - bias <= s, 0.25);

	float3 normal = gbuffer0.Sample(sm_point, uv).xyz;
	float3 colour = gbuffer1.Sample(sm_point, uv).xyz;

    float radiance = lighted * max(0.0, dot( lightvec, normal )) / (l * l * l) * light_scale;

	return float4(radiance * light_colour * colour, 1.0);
}

float4 ps_final(float2 uv : Position, float4 pos : SV_Position) : SV_Target0
{
	if (zbuffer.Sample(sm_point, uv).x == 1.0) //sky
	{
		float4 horizon = float4(0.6, 0.75, 0.9, 1.0);
		float4 zenith = float4(0.25, 0.35, 0.9, 1.0);

		float3 world_ray = mul( (float3x3)view_i, float3(uv_to_ray(uv), 1.0) );
		return lerp(horizon, zenith, -world_ray.z );
	}
	
	float3 normal = gbuffer0.Sample(sm_point, uv).xyz;
	float3 colour = gbuffer1.Sample(sm_point, uv).xyz;
	float mult = max(0.0, dot( view_i[2].xyz, normal));
	float4 ambient = float4(mult * float3(0.02, 0.02, 0.02) * colour, 1.0);

	return (ambient + lbuffer.Sample(sm_point, uv)) / aperture;
}
