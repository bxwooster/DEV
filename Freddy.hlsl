cbuffer frame	
{
	float4x4 mViewProj;
	float4x4 mView_I;
	float aperture;
	float field_of_view;
	float aspect_ratio;
}

cbuffer object
{
	float4x4 mWorldViewProj;
	float4x4 mWorldView;
}

cbuffer light
{
	float3 light_pos;
	float3 light_colour;
}

Texture2D gbuffer0;
Texture2D gbuffer1;
Texture2D zbuffer;
Texture2D accum;

sampler smp;

const float light_scale = 8.0;
const float eps = 3e-7;

RasterizerState rs_default
{
	FrontCounterClockwise = true; 
	CullMode = none;
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
	float zn = 0.01;
	return zn / ( depth - 1.0 );
}

float2 uv_to_ray(float2 uv)
{
	float alpha = radians(field_of_view) * 0.5;
	uv = uv * 2.0 - 1.0;
	return uv * float2(-aspect_ratio, 1.0) * tan(alpha);
}

void vs_render( uint id : SV_VertexID, Vertex vertex, out Pixel pixel )
{
	pixel.pos = mul( mWorldViewProj, vertex.position );
	pixel.normal = normalize( mul( (float3x3)mWorldView, vertex.normal ) );
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
	float z = zbuffer.Sample(smp, uv).x;
	clip(1.0 - eps - z);
	z = linear_z( z );
	float4 view_pos = float4( view_ray * z, z, 1.0 );
float3 normal = gbuffer0.Sample(smp, uv).xyz;
	float3 colour = gbuffer1.Sample(smp, uv).xyz;
	float3 lightvec = light_pos.xyz - view_pos.xyz;
	float len = length(lightvec);
    float radiance = max(0.0, dot( lightvec, normal )) / (len * len * len) * light_scale;

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

	float3 world_ray = mul( (float4x3)mView_I, float3(view_ray, 1.0) );
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