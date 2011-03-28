float4x4 view;
float interp;
float field_of_view;
float aspect_ratio;
int steps = 128;

RasterizerState rs_default
{
	FrontCounterClockwise = True; 
};

struct Pixel
{
	float2 uv : UV;
	float4 pos : SV_Position;
};

float2 uv_to_ray(float2 uv)
{
	float alpha = radians(field_of_view) * 0.5;
	return uv * float2(-aspect_ratio, 1.0) * tan(alpha);
}

float distance_to_surface(float3 world)
{
	float torus = length(float2(length(world.xy) - 1.0, world.z)) - 0.25;
	float cube = max(max(abs(world.x), abs(world.y)), abs(world.z)) - 1.0;
	float sphere = length(world) - sqrt(2.0);
	float cylinder = max(length(world.xy) - 1.0, abs(world.z) - 1.0);

	float t = 1;
	float t2 = 0.25;
	float spheres = (length(frac(world * t + 0.5) - 0.5) - 0.25) / t;
	float spheres2 = (length(frac(world * t2 + 0.5) - 0.5) - 0.25) / t2;

	return lerp(cube, spheres, interp);
	//return cube;
}

float4 ps_surface(float2 uv : UV) : SV_Target0
{
	float4 pos = float4(mul( view, float4(0.0, 0.0, 4.5, 0.0) ).xyz, 0.0);
	//float4 pos = float4(0.0, 0.0, 4.5, 0.0);
	float4 ray = float4(mul( view, -normalize(float4(uv_to_ray(uv), 1.0, 0.0) ) ).xyz, 1.0);
	//float4 ray = float4(mul( view, -float4(uv.x, uv.y, 1.0 - dot(uv, uv), 1.0) ).xyz, 1.0);

	float dist = 1.0;
	for(int i = 0; i < steps && dist / pos.w > 0.0025; i++)
	{
		dist = distance_to_surface(pos.xyz);
		//clip( surface_far - ray.w );

		pos += ray * dist;
	}

	//clip((steps - i - 1));

	float eps = 1e-3;
	float3 normal;
	dist = distance_to_surface(pos.xyz);
	normal.x = distance_to_surface(pos.xyz + float3(eps, 0, 0)) - dist;
	normal.y = distance_to_surface(pos.xyz + float3(0, eps, 0)) - dist;
	normal.z = distance_to_surface(pos.xyz + float3(0, 0, eps)) - dist;
	normal = normalize(normal);

	float3 lightdir = float3(0.7, 0.5, 0.5);

	float ao = 1.0;
	for (int k = 1; k < 5; ++k)
	{
		ao -= 5 * (0.1 * k - distance_to_surface(pos + float4(normal, 0.0) * 0.1 * k)) / pow(2, k);
	};

	//return log2(pos.w) / 5;
	//return dot(lightdir, normal);
	return float4(ao*normal, 1.0);
}

void vs_fullscreen(uniform float depth, float2 position : POSITION, out Pixel pixel)
{
	pixel.pos = float4(position * 2.0 - 1.0, depth, 1.0);
	pixel.uv = float2(position.x, 1.0 - position.y) * 2.0 - 1.0;
}

technique11
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, vs_fullscreen(1.0) ) );
		SetPixelShader( CompileShader( ps_4_0, ps_surface() ) );
		SetRasterizerState( rs_default );
	}
}

