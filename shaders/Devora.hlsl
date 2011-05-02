cbuffer frame	
{
	float4x4 view_i;
	float z_near;
	float aperture;
	float field_of_view;
	float aspect_ratio;
};

cbuffer object
{
	float4x4 world_view_proj;
	float4x4 world_view;
};

cbuffer object_z
{
	float4x4 world_lightview;
	float4x4 world_lightview_lightproj;
};

cbuffer light
{
	float3 light_pos;
	float3 light_colour;
	float4x4 light_matrix;
	float4x4 reproject;
};

float4x4 cubeproj[6];

Texture2D gbuffer0;
Texture2D gbuffer1;
Texture2D zbuffer;
Texture2D shadowmap;
TextureCube shadowcube;
Texture2D lbuffer;

sampler smp;
static const float bias = 0.2;
static const float light_scale = 200.0;
static const float eps = 3e-7;

RasterizerState rs_default
{
	FrontCounterClockwise = true; 
};

RasterizerState rs_shadow
{
	FrontCounterClockwise = true; 
	DepthBias = 0;
	SlopeScaledDepthBias = 1.0;
};

BlendState bs_default;
BlendState bs_additive
{
	BlendEnable[0] = true;
	DestBlend = one;
};

DepthStencilState ds_default;
DepthStencilState ds_nowrite
{
	DepthWriteMask = zero;
	DepthFunc = less_equal;
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
	//float2 view_ray : Ray;
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
	float alpha = radians(field_of_view) * 0.5;
	uv = uv * 2.0 - 1.0;
	return uv * float2(-aspect_ratio, 1.0) * tan(alpha);
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

void vs_dummy( out Empty empty )
{

}

[maxvertexcount(3)]
void gs_fullscreen(uniform float depth, point Empty empty[1], inout TriangleStream<ScreenPixel> stream)
{
	ScreenPixel pixel;
	float2 position;
	for (uint id = 0; id < 3; id++)
	{
		if (id == 0) position = float2(0, 0);
		if (id == 1) position = float2(2, 0);
		if (id == 2) position = float2(0, 2);

		pixel.pos = float4(position * 2.0 - 1.0, depth, 1.0);
		pixel.uv = float2(position.x, 1.0 - position.y);

		stream.Append(pixel);
	}
}

float4 ps_directional_light(float2 uv : Position, float4 pos : SV_Position) : SV_Target0
{
	int3 uvw = int3(pos.xy, 0);
	float z_neg = -z_near / (1.0 - zbuffer.Load(uvw));
	float4 surface_pos = float4( uv_to_ray(uv) * z_neg, z_neg, 1.0 );
	float4 reprojected = mul(reproject, surface_pos);
	float in_front = reprojected.w > 0;
	reprojected /= reprojected.w;
	float inside_cone = length(reprojected.xy) < 1;
	clip(in_front * inside_cone - 1);
	float2 s_uv = float2(reprojected.x, -reprojected.y) * 0.5 + 0.5;
	float4 s = z_near / (1.0 - shadowmap.Gather(smp, s_uv).x);

	float3 lightvec = light_pos - surface_pos.xyz;
	float l = length(lightvec);
	float p = dot(light_matrix[2].xyz, lightvec);
	float lighted = dot(p - bias <= s, 0.25);

	float3 normal = gbuffer0.Load(uvw).xyz;
	float3 colour = gbuffer1.Load(uvw).xyz;

    float radiance = lighted * max(0.0, dot( lightvec, normal )) / (l * l * l) * light_scale;

	return float4(radiance * light_colour * colour, 1.0);
}

float4 ps_point_light(float2 uv : Position, float4 pos : SV_Position) : SV_Target0
{
	int3 uvw = int3(pos.xy, 0);
	float z_neg = -z_near / (1.0 - zbuffer.Load(uvw).x);
	float4 surface_pos = float4( uv_to_ray(uv) * z_neg, z_neg, 1.0 );
	float4 reprojected = mul(reproject, surface_pos);
	//reprojected /= reprojected.w;
	float4 s = z_near / (1.0 - shadowcube.Gather(smp, reprojected.xyz));

	float3 lightvec = light_pos - surface_pos.xyz;
	float l = length(lightvec);
	float3 p = abs(mul(light_matrix, float4(lightvec, 0)).xyz);
	float m = max(p.x, max(p.y, p.z));
	float lighted = dot(m - bias <= s, 0.25);

	float3 normal = gbuffer0.Load(uvw).xyz;
	float3 colour = gbuffer1.Load(uvw).xyz;

    float radiance = lighted * max(0.0, dot( lightvec, normal )) / (l * l * l) * light_scale;

	return float4(radiance * light_colour * colour, 1.0);
}

float4 ps_final(float2 uv : Position, float4 pos : SV_Position) : SV_Target0
{
	int3 uvw = int3(pos.xy, 0);
	if (zbuffer.Sample(smp, uv).x == 1.0) //sky
	{
		float4 horizon = float4(0.6, 0.75, 0.9, 1.0);
		float4 zenith = float4(0.25, 0.35, 0.9, 1.0);

		float3 world_ray = mul( (float3x3)view_i, float3(uv_to_ray(uv), 1.0) );
		return lerp(horizon, zenith, -world_ray.z );
	}
	
	float3 normal = gbuffer0.Load(uvw).xyz;
	float3 colour = gbuffer1.Load(uvw).xyz;
	float mult = max(0.0, dot( view_i[2].xyz, normal));
	float4 ambient = float4(mult * light_colour * colour, 1.0);

	return (ambient + lbuffer.Load(uvw)) / aperture;
}

technique11 render
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_render() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_1, ps_render() ) );
		SetRasterizerState( rs_default );
		SetBlendState( bs_default, float4(1.0, 1.0, 1.0, 0.0), 0xffffffff );
		SetDepthStencilState( ds_default, 0 );
	}
}

technique11 render_z
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_render_z() ) );
		SetGeometryShader( NULL );
		SetPixelShader( NULL );
		SetBlendState( bs_default, float4(1.0, 1.0, 1.0, 0.0), 0xffffffff );
		SetDepthStencilState( ds_default, 0 );
		SetRasterizerState( rs_shadow );
	}
}

technique11 render_cube_z
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_render_cube_z() ) );
		SetGeometryShader( CompileShader( gs_4_1, gs_render_cube_z() ) );
		SetPixelShader( NULL );
		SetBlendState( bs_default, float4(1.0, 1.0, 1.0, 0.0), 0xffffffff );
		SetDepthStencilState( ds_default, 0 );
		SetRasterizerState( rs_shadow );
	}
}

technique11 directional_light
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_dummy() ) );
		SetGeometryShader( CompileShader( gs_4_1, gs_fullscreen(0.0) ) );
		SetPixelShader( CompileShader( ps_4_1, ps_directional_light() ) );
		SetBlendState( bs_additive, float4(1.0, 1.0, 1.0, 0.0), 0xffffffff );
		SetDepthStencilState( ds_nowrite, 0 );
		SetRasterizerState( rs_default );
	}
}

technique11 point_light
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_dummy() ) );
		SetGeometryShader( CompileShader( gs_4_1, gs_fullscreen(0.0) ) );
		SetPixelShader( CompileShader( ps_4_1, ps_point_light() ) );
		SetBlendState( bs_additive, float4(1.0, 1.0, 1.0, 0.0), 0xffffffff );
		SetDepthStencilState( ds_nowrite, 0 );
		SetRasterizerState( rs_default );
	}
}

technique11 final
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_1, vs_dummy() ) );
		SetGeometryShader( CompileShader( gs_4_1, gs_fullscreen(0.0) ) );
		SetPixelShader( CompileShader( ps_4_1, ps_final() ) );
		SetBlendState( bs_default, float4(1.0, 1.0, 1.0, 0.0), 0xffffffff );
		SetDepthStencilState( ds_nowrite, 0 );
		SetRasterizerState( rs_default );
	}
}