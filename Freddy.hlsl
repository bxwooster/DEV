cbuffer frame	
{
	float4x4 view_proj;
	float4x4 view_i;
	float4x4 view;
	float z_near;
	float aperture;
	float field_of_view;
	float aspect_ratio;
}

cbuffer object
{
	float4x4 world_view_proj;
	float4x4 world_view;
}

cbuffer object_z
{
	float4x4 world_lightview_lightproj;
}

cbuffer light
{
	float3 light_pos;
	float3 light_colour;
	float4x4 reproject;
}

Texture2D gbuffer0;
Texture2D gbuffer1;
Texture2D zbuffer;
Texture2D shadowmap;
Texture2D accum;

sampler smp;

static const float light_scale = 10.0;
static const float eps = 3e-7;

RasterizerState rs_default
{
	FrontCounterClockwise = true; 
};

BlendState bs_none
{
	BlendEnable[0] = false;
};

BlendState bs_additive
{
	BlendEnable[0] = true;
	DestBlend = one;
};

DepthStencilState ds_nowrite
{
	DepthWriteMask = zero;
	DepthFunc = less_equal;
};

DepthStencilState ds_tmp
{
	DepthFunc = always;
};

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
	float2 view_ray : Ray;
	float4 pos : SV_Position;
};

float linear_z(float depth)
{
	return z_near / ( 1.0 - depth);
}

float2 uv_to_ray(float2 uv)
{
	float alpha = radians(field_of_view) * 0.5;
	uv = uv * 2.0 - 1.0;
	return uv * float2(-aspect_ratio, 1.0) * tan(alpha);
}

void vs_render( Vertex vertex, out Pixel pixel )
{
	pixel.pos = mul( world_view_proj, vertex.position );
	pixel.normal = normalize( mul( world_view, vertex.normal ) );
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

void vs_fullscreen(uniform float depth, float2 position : POSITION, out ScreenPixel pixel)
{
	pixel.pos = float4(position * 2.0 - 1.0, depth, 1.0);
	pixel.uv = float2(position.x, 1.0 - position.y);
	pixel.view_ray = uv_to_ray(pixel.uv);
}

float4 ps_directional_light(float2 uv : Position, float2 view_ray : Ray) : SV_Target0
{
	return float4(light_colour * (shadowmap.Sample(smp, uv).x - 0.9) / 0.1, 1.0);

	float z_neg = -linear_z(zbuffer.Sample(smp, uv).x);
	float4 surface_pos = float4( view_ray * z_neg, z_neg, 1.0 );
	float4 reprojected = mul(reproject, surface_pos);
	float in_front = reprojected.w > 0;
	reprojected /= reprojected.w;
	float inside_cone = length(reprojected.xy) < 1;
	float s = linear_z(shadowmap.Sample(smp, reprojected.xy * 0.5 + 0.5).x);

	float3 lightvec = light_pos - surface_pos.xyz;
	float l = length(lightvec);

	float3 normal = gbuffer0.Sample(smp, uv).xyz;
	float3 colour = gbuffer1.Sample(smp, uv).xyz;

	float bias = 0.3;
    float radiance = (l < s + bias) * in_front * inside_cone * max(0.0, dot( lightvec, normal )) / (l * l * l) * light_scale;

	return float4(radiance * light_colour * colour, 1.0);
}

float4 ps_ambient_light(float2 uv : Position) : SV_Target0
{
	float3 colour = gbuffer1.Sample(smp, uv).xyz;
	return float4(light_colour * colour, 1.0);
}

float4 ps_sky(float2 uv : Position, float2 view_ray : Ray) : SV_Target0
{
	float4 horizon = float4(0.6, 0.75, 0.9, 1.0);
	float4 zenith = float4(0.25, 0.35, 0.9, 1.0);

	float3 world_ray = mul( (float4x3)view_i, float3(view_ray, 1.0) );
	return lerp(horizon, zenith, dot( world_ray, float3(0.0, 0.0, -1.0) ) );
}

float4 ps_hdr(float2 uv : Position) : SV_Target0
{
	return accum.Sample(smp, uv) / aperture;
}

technique11 render
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_render() ) );
		SetPixelShader( CompileShader( ps_4_1, ps_render() ) );
		SetRasterizerState( rs_default );
	}
}

technique11 render_z
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_render_z() ) );
		SetPixelShader( NULL );
		SetRasterizerState( rs_default );
	}
}


technique11 ambient_light
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_fullscreen(0.0) ) );
		SetPixelShader( CompileShader( ps_4_1, ps_ambient_light() ) );
		SetBlendState( bs_additive, float4(1.0, 1.0, 1.0, 0.0), 0xffffffff );
		SetDepthStencilState( ds_nowrite, 0 );
		SetRasterizerState( rs_default );
	}
}

technique11 directional_light
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_fullscreen(0.0) ) );
		SetPixelShader( CompileShader( ps_4_1, ps_directional_light() ) );
		SetBlendState( bs_additive, float4(1.0, 1.0, 1.0, 0.0), 0xffffffff );
		SetDepthStencilState( ds_nowrite, 0 );
		SetRasterizerState( rs_default );
	}
}

technique11 sky
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_fullscreen(1.0) ) );
		SetPixelShader( CompileShader( ps_4_1, ps_sky() ) );
		SetBlendState( bs_none, float4(1.0, 1.0, 1.0, 0.0), 0xffffffff );
		SetDepthStencilState( ds_nowrite, 0 );
		SetRasterizerState( rs_default );
	}
}

technique11 hdr
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_fullscreen(0.0) ) );
		SetPixelShader( CompileShader( ps_4_1, ps_hdr() ) );
		SetRasterizerState( rs_default );
	}
}