#include "Camera.hpp"
#include "DeviceState.hpp"
#include "GraphicsState.hpp"
#include "VisualRenderInfo.hpp"
#include "LightRenderInfo.hpp"
#include "PostProcessInfo.hpp"
#include "Buffer.hpp"
#include "ZBuffer.hpp"
#include "CBuffer.hpp"
#include "CBufferLayouts.hpp"

#include <D3DX11.h>

namespace Devora {
namespace Tools {

void SetProjectionMatrix(Matrix4f& proj, float y_fov, float aspect_ratio, float z_near);
Geometry ReadGeometry(ID3D11Device* device, const std::string& filename);

}; using namespace Tools;

LRESULT CALLBACK WindowProc(HWND handle, UINT msg, WPARAM w, LPARAM l)
{
	switch (msg) 
	{ 
		case WM_SETFOCUS:
			ShowCursor(false);
			RECT rect;
			GetWindowRect( handle, &rect );
			ClipCursor( &rect );
			return 0;
		case WM_KILLFOCUS:
			ShowCursor(true);
			ClipCursor( NULL );
			return 0;
		case WM_CLOSE:
			throw 0;
	} 
	return DefWindowProc(handle, msg, w, l);
}

void CompileShader( char* file, char* entry, char* profile, ID3D10Blob** code )
{
	IPtr<ID3D10Blob> info;

	unsigned int shader_flags = D3D10_SHADER_ENABLE_STRICTNESS |
	D3D10_SHADER_OPTIMIZATION_LEVEL0 |
	D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

	HOK_EX( D3DX11CompileFromFile( file,
	NULL, NULL, entry, profile, shader_flags,
	0, NULL, code, &info, NULL ),
	*&info ? (char*)info->GetBufferPointer() : "" );
}

void InitGraphics(GraphicsState& state, DeviceState& device, 
	VisualRenderInfo& vinfo, LightRenderInfo& linfo, PostProcessInfo& pinfo,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& lbuffer, ZBuffer& zbuffer, Buffer& backbuffer, Camera& camera,
	CBuffer& cb_object, CBuffer& cb_object_z, CBuffer& cb_object_cube_z, 
	CBuffer& cb_light, CBuffer& cb_frame)
{
	device.z_near = 0.2f;
	device.shadowmap_size = 512;
	device.width = device.height = 960;

	WNDCLASSEX window_class;
	ZeroMemory( &window_class, sizeof( WNDCLASSEX ) );
	window_class.lpszClassName = "Devora::window";                        
	window_class.cbSize = sizeof( WNDCLASSEX );      
	window_class.lpfnWndProc = WindowProc;  
	RegisterClassEx( &window_class );

	OK( device.window = CreateWindowEx( 0, window_class.lpszClassName, "Devora",
		WS_SYSMENU | WS_OVERLAPPED | WS_VISIBLE, CW_USEDEFAULT,
		CW_USEDEFAULT, device.width, device.height, NULL, NULL, NULL, 0 ) );

	//Misc
	{
		linfo.cubematrices[0] <<
				0, 0, 1, 0,
				0, 1, 0, 0,
				-1, 0, 0, 0,
				0, 0, 0, 1;
		linfo.cubematrices[1] <<
				0, 0,-1, 0,
				0, 1, 0, 0,
				1, 0, 0, 0,
				0, 0, 0, 1;
		linfo.cubematrices[2] <<
				1, 0, 0, 0,
				0, 0, 1, 0,
				0,-1, 0, 0,
				0, 0, 0, 1;
		linfo.cubematrices[3] <<
				1, 0, 0, 0,
				0, 0,-1, 0,
				0, 1, 0, 0,
				0, 0, 0, 1;
		linfo.cubematrices[4] <<
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1;
		linfo.cubematrices[5] <<
				-1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0,-1, 0,
				0, 0, 0, 1;

		SetProjectionMatrix(linfo.proj, 90, 1.0, device.z_near);
	}

	// Device, Factory
	{
		IPtr<IDXGIDevice1> dxgi_device;
		IPtr<IDXGIAdapter1> dxgi_adapter;
		IPtr<IDXGIFactory1> dxgi_factory;

		D3D_FEATURE_LEVEL feature_level;

#ifdef _DEBUG
		D3D10_CREATE_DEVICE_FLAG flag = D3D10_CREATE_DEVICE_DEBUG;
#else
		D3D10_CREATE_DEVICE_FLAG flag = D3D10_CREATE_DEVICE_FLAG(0);
#endif
		HOK( D3D11CreateDevice
			( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flag,
			NULL, 0, D3D11_SDK_VERSION, &device.device, &feature_level, &state.context ) );

		HOK( device.device->QueryInterface
			( __uuidof(IDXGIDevice1), (void**)&dxgi_device ) );
    
		HOK( dxgi_device->GetParent
			( __uuidof(IDXGIAdapter1), (void**)&dxgi_adapter ) );
    
		HOK( dxgi_adapter->GetParent
			( __uuidof(IDXGIFactory1), (void**)&dxgi_factory ) );
	
		HOK( dxgi_factory->MakeWindowAssociation(NULL, DXGI_MWA_NO_WINDOW_CHANGES) );

		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferCount = 2;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferDesc.Width = device.width;
		desc.BufferDesc.Height = device.height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.OutputWindow = device.window;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = 1;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HOK( dxgi_factory->CreateSwapChain
			( device.device, &desc, &device.swap_chain ));
	}

	// Back buffer, view
	{
		IPtr<ID3D11Texture2D> buffer;
	
		HOK( device.swap_chain->GetBuffer
			( 0, __uuidof( ID3D11Texture2D ), ( void** )( &backbuffer.texture ) ));

		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT(0);
		desc.Texture2D.MipSlice = 0;

		HOK( device.device->CreateRenderTargetView
			( backbuffer.texture, NULL, &backbuffer.rtv ));
	}

	// Effect, Variables, Passes
	{
		IPtr<ID3DX11Effect> effect;
		IPtr<ID3D10Blob> code;

		CompileShader( "shaders/Devora.hlsl", NULL, "fx_5_0", &code );
		HOK( D3DX11CreateEffectFromMemory( code->GetBufferPointer(),
			code->GetBufferSize(), 0, device.device, &effect ));

		state.effect = effect;

		state.pass_render = effect->GetTechniqueByName("render")->GetPassByIndex(0);
		state.pass_render_z = effect->GetTechniqueByName("render_z")->GetPassByIndex(0);
		state.pass_render_cube_z = effect->GetTechniqueByName("render_cube_z")->GetPassByIndex(0);
		state.pass_directional_light = effect->GetTechniqueByName("directional_light")->GetPassByIndex(0);
		state.pass_point_light = effect->GetTechniqueByName("point_light")->GetPassByIndex(0);
		state.pass_final = effect->GetTechniqueByName("final")->GetPassByIndex(0);

		state.var.lbuffer = effect->GetVariableByName("lbuffer")->AsShaderResource();
		state.var.zbuffer = effect->GetVariableByName("zbuffer")->AsShaderResource();
		state.var.shadowmap = effect->GetVariableByName("shadowmap")->AsShaderResource();
		state.var.shadowcube = effect->GetVariableByName("shadowcube")->AsShaderResource();
		state.var.gbuffer0 = effect->GetVariableByName("gbuffer0")->AsShaderResource();
		state.var.gbuffer1 = effect->GetVariableByName("gbuffer1")->AsShaderResource();
		state.var.aperture = effect->GetVariableByName("aperture")->AsScalar();
		state.var.ambient = effect->GetVariableByName("ambient")->AsVector();
		state.var.z_near = effect->GetVariableByName("z_near")->AsScalar();
		state.var.xy_to_ray = effect->GetVariableByName("xy_to_ray")->AsVector();
		state.var.light_colour = effect->GetVariableByName("light_colour")->AsVector();
		state.var.light_pos = effect->GetVariableByName("light_pos")->AsVector();
		state.var.light_matrix = effect->GetVariableByName("light_matrix")->AsMatrix();
		state.var.view_i = effect->GetVariableByName("view_i")->AsMatrix();
		state.var.reproject = effect->GetVariableByName("reproject")->AsMatrix();
		state.var.cubeproj = effect->GetVariableByName("cubeproj")->AsMatrix();
		state.var.world_view = effect->GetVariableByName("world_view")->AsMatrix();
		state.var.world_view_proj = effect->GetVariableByName("world_view_proj")->AsMatrix();
		state.var.world_lightview_lightproj = effect->GetVariableByName("world_lightview_lightproj")->AsMatrix();
	}

	// Layout
	{
		D3DX11_PASS_DESC desc;
		HOK( state.pass_render->GetDesc( &desc ) );
        
		D3D11_INPUT_ELEMENT_DESC element[2] =
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},

			{
				"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			}
		};

		HOK( device.device->CreateInputLayout
			( element, 2, desc.pIAInputSignature,
				desc.IAInputSignatureSize, &vinfo.layout ) );
	}

	// Textures
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc) );
		desc.Width = device.shadowmap_size;
		desc.Height = device.shadowmap_size;
		desc.Format = DXGI_FORMAT_R16_TYPELESS;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		desc.ArraySize = 6;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MipLevels = 1;
		desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

		HOK( device.device->CreateTexture2D( &desc, NULL, &shadowcube.texture ) );
		desc.ArraySize = 1;
		desc.MiscFlags = 0;
		HOK( device.device->CreateTexture2D( &desc, NULL, &shadowmap.texture ) );

		desc.Width = device.width;
		desc.Height = device.height;
		HOK( device.device->CreateTexture2D( &desc, NULL, &zbuffer.texture ) );

		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		HOK( device.device->CreateTexture2D( &desc, NULL, &gbuffer0.texture ) );
		HOK( device.device->CreateTexture2D( &desc, NULL, &lbuffer.texture ) );

		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		HOK( device.device->CreateTexture2D( &desc, NULL, &gbuffer1.texture ) );
	}

	// Views
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT_R16_UNORM;
		desc.Texture2D.MipLevels = 1;
		desc.Texture2D.MostDetailedMip = 0;

		HOK( device.device->CreateShaderResourceView
			( shadowmap.texture, &desc, &shadowmap.srv ));

		HOK( device.device->CreateShaderResourceView
			( zbuffer.texture, &desc, &zbuffer.srv ));

		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels = 1;
		desc.TextureCube.MostDetailedMip = 0;

		HOK( device.device->CreateShaderResourceView
		  ( shadowcube.texture, &desc, &shadowcube.srv ));
	}
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT_D16_UNORM;
		desc.Texture2D.MipSlice = 0;

		HOK( device.device->CreateDepthStencilView
			( shadowmap.texture, &desc, &shadowmap.dsv ));

		HOK( device.device->CreateDepthStencilView
		  ( zbuffer.texture, &desc, &zbuffer.dsv ));

		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = 0;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.ArraySize = 6;

		HOK( device.device->CreateDepthStencilView
		  ( shadowcube.texture, &desc, &shadowcube.dsv ));
	}
	HOK( device.device->CreateShaderResourceView
		( gbuffer0.texture, NULL, &gbuffer0.srv ) );
	HOK( device.device->CreateShaderResourceView
		( gbuffer1.texture, NULL, &gbuffer1.srv ) );
	HOK( device.device->CreateShaderResourceView
		( lbuffer.texture, NULL, &lbuffer.srv ) );
	HOK( device.device->CreateRenderTargetView
		( gbuffer0.texture, NULL, &gbuffer0.rtv ) );
	HOK( device.device->CreateRenderTargetView
		( gbuffer1.texture, NULL, &gbuffer1.rtv ) );
	HOK( device.device->CreateRenderTargetView
		( lbuffer.texture, NULL, &lbuffer.rtv ) );

	// Viewports
	{			
		gbuffer0.viewport.Width = float(device.width);
		gbuffer0.viewport.Height = float(device.height);
		gbuffer0.viewport.MinDepth = 0.0f;
		gbuffer0.viewport.MaxDepth = 1.0f;
		gbuffer0.viewport.TopLeftX = 0.0f;
		gbuffer0.viewport.TopLeftY = 0.0f;

		backbuffer.viewport = lbuffer.viewport = zbuffer.viewport 
			= gbuffer1.viewport = gbuffer0.viewport;
	   
		shadowmap.viewport.Width = float(device.shadowmap_size);
		shadowmap.viewport.Height = float(device.shadowmap_size);
		shadowmap.viewport.MinDepth = 0.0f;
		shadowmap.viewport.MaxDepth = 1.0f;
		shadowmap.viewport.TopLeftX = 0.0f;
		shadowmap.viewport.TopLeftY = 0.0f;

		shadowcube.viewport = shadowmap.viewport;
	}

	// Geometry
	vinfo.geoms.push_back( ReadGeometry(device.device, "geometry//plane.geom") );
	vinfo.geoms.push_back( ReadGeometry(device.device, "geometry//icosphere.geom") );

	// Shaders
	{
		IPtr<ID3D11ClassLinkage> linkage;
		IPtr<ID3D10Blob> code;
		HOK( device.device->CreateClassLinkage( &linkage ));

		CompileShader( "shaders/Devora.hlsl", "vs_render", "vs_5_0", &code );
		HOK( device.device->CreateVertexShader(code->GetBufferPointer(),
			code->GetBufferSize(), linkage, &vinfo.vs_render));
		CompileShader( "shaders/Devora.hlsl", "ps_render", "ps_5_0", &code );
		HOK( device.device->CreatePixelShader(code->GetBufferPointer(),
			code->GetBufferSize(), linkage, &vinfo.ps_render));

		CompileShader( "shaders/Devora.hlsl", "vs_render_z", "vs_5_0", &code );
		HOK( device.device->CreateVertexShader(code->GetBufferPointer(),
			code->GetBufferSize(), linkage, &linfo.vs_render_z));

		CompileShader( "shaders/Devora.hlsl", "vs_render_cube_z", "vs_5_0", &code );
		HOK( device.device->CreateVertexShader(code->GetBufferPointer(),
			code->GetBufferSize(), linkage, &linfo.vs_render_cube_z));
		CompileShader( "shaders/Devora.hlsl", "gs_render_cube_z", "gs_5_0", &code );
		HOK( device.device->CreateGeometryShader(code->GetBufferPointer(),
			code->GetBufferSize(), linkage, &linfo.gs_render_cube_z));

		CompileShader( "shaders/Devora.hlsl", "vs_noop", "vs_5_0", &code );
		HOK( device.device->CreateVertexShader(code->GetBufferPointer(),
			code->GetBufferSize(), linkage, &linfo.vs_noop));
		CompileShader( "shaders/Devora.hlsl", "gs_fullscreen", "gs_5_0", &code );
		HOK( device.device->CreateGeometryShader(code->GetBufferPointer(),
			code->GetBufferSize(), linkage, &linfo.gs_fullscreen));

		CompileShader( "shaders/Devora.hlsl", "ps_dir_light", "ps_5_0", &code );
		HOK( device.device->CreatePixelShader(code->GetBufferPointer(),
			code->GetBufferSize(), linkage, &linfo.ps_dir_light));
		CompileShader( "shaders/Devora.hlsl", "ps_point_light", "ps_5_0", &code );
		HOK( device.device->CreatePixelShader(code->GetBufferPointer(),
			code->GetBufferSize(), linkage, &linfo.ps_point_light));

		CompileShader( "shaders/Devora.hlsl", "ps_final", "ps_5_0", &code );
		HOK( device.device->CreatePixelShader(code->GetBufferPointer(),
			code->GetBufferSize(), linkage, &pinfo.ps_final));

		pinfo.vs_noop = linfo.vs_noop;
		pinfo.gs_fullscreen = linfo.gs_fullscreen;
	}

	// States
	{
		D3D11_RASTERIZER_DESC desc;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;
		//
		desc.FrontCounterClockwise = true;
		HOK( device.device->CreateRasterizerState( &desc, &vinfo.rs_default));
		pinfo.rs_default = vinfo.rs_default;

		desc.SlopeScaledDepthBias = 1.0f;
		HOK( device.device->CreateRasterizerState( &desc, &linfo.rs_shadow));
	}
	{
		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = false;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		//
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		HOK( device.device->CreateBlendState( &desc, &linfo.bs_additive));
	}
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = false;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		//
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		HOK( device.device->CreateDepthStencilState( &desc, &linfo.ds_nowrite));
	}

	// Samplers
	{
		float zeros[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MipLODBias = 0.0f;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.MaxAnisotropy = 16;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		*desc.BorderColor = *zeros;
		//
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		HOK( device.device->CreateSamplerState( &desc, &linfo.sm_point));
		pinfo.sm_point = linfo.sm_point;
	}


	// Constant Buffers
	{
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		desc.ByteWidth = sizeof( CBufferLayouts::object );
		HOK( device.device->CreateBuffer( &desc, NULL, &cb_object ));

		desc.ByteWidth = sizeof( CBufferLayouts::frame );
		HOK( device.device->CreateBuffer( &desc, NULL, &cb_frame ));

		desc.ByteWidth = sizeof( CBufferLayouts::object_z );
		HOK( device.device->CreateBuffer( &desc, NULL, &cb_object_z ));

		desc.ByteWidth = sizeof( CBufferLayouts::object_cube_z );
		HOK( device.device->CreateBuffer( &desc, NULL, &cb_object_cube_z ));

		desc.ByteWidth = sizeof( CBufferLayouts::light );
		HOK( device.device->CreateBuffer( &desc, NULL, &cb_light ));
	}

	HOK( state.var.z_near->SetFloat( device.z_near ) ); //!
}

} // namespace Devora
