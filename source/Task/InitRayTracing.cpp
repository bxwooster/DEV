#define NOMINMAX
#include "InitRayTracing.hpp"
#include "OK.hpp"
#include "Tools.hpp"
#include "LoadShader.hpp"

namespace DEV {

void InitRayTracing::run()
{
	LoadShader::Vertex(cache, device, info.vs_noop, "shaders/vs_noop.hlsl");
	LoadShader::Geometry(cache, device, info.gs_fullscreen, "shaders/gs_fullscreen.hlsl");
	LoadShader::Pixel(cache, device, info.ps_tracy, "shaders/ps_tracy.hlsl");

	D3D11_RASTERIZER_DESC desc = Tools::DefaultRasterizerDesc();
	HOK( device.device->CreateRasterizerState( &desc, ~info.rs_default));
}

} // namespace DEV