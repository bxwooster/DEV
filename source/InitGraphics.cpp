#include "Camera.hpp"
#include "DeviceState.hpp"
#include "GraphicsState.hpp"
#include "VisualRenderInfo.hpp"
#include "LightRenderInfo.hpp"
#include "PostProcessInfo.hpp"
#include "Buffer.hpp"
#include "ZBuffer.hpp"

#include <D3DX11.h>
#include <fstream>

namespace Devora {

namespace {
	typedef unsigned int uint;

	const float to_radians = (float)(M_PI / 180.0);
	const float from_radians = (float)(180.0 / M_PI);

	void projection_matrix(Matrix4f& proj, float y_fov, float aspect_ratio, float z_near)
	{
		float ys = 1 / tanf( y_fov * to_radians / 2.0f );
		float xs = ys / aspect_ratio;
		proj << xs, 0, 0, 0,
				0, ys, 0, 0,
				0, 0, -1, -z_near,
				0, 0, -1, 0;
	}

	Geometry read_geom(DeviceState& device, const std::string& filename)
	{
		using std::ifstream;

		Geometry geometry;

		ifstream file( filename, ifstream::in | ifstream::binary );
		file.exceptions( ifstream::eofbit | ifstream::failbit | ifstream::badbit );

		char head[8];
		char ver[4];
    
		file.read( head, 8 );
		file.read( ver, 4 );

		uint elements;
		file.read( (char*)&elements, 4 );

		uint fmt, index, len;

		for ( uint i = 0; i < elements; i++ )
		{
			file.read( (char*)&fmt, 4 );
			file.read( (char*)&index, 4 );
			file.read( (char*)&len, 4 );
        
			char* semantic = new char[len + 1];
			file.read( (char*)semantic, len );
			delete[] semantic;
		}
    
		geometry.offset = 0;
		file.read( (char*)&geometry.stride, 4 );
    
		uint width;
		file.read( (char*)&width, 4 );
		geometry.count = width / geometry.stride;
    
		char* vertices = new char[width];
		file.read( vertices, width ); 

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = vertices;

		D3D11_BUFFER_DESC buffer_desc;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.ByteWidth = width;

		HOK( device.device->CreateBuffer
		  ( &buffer_desc, &data, &geometry.buffer ) );

		delete[] vertices;

		return geometry;
	}

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
}

void InitGraphics(GraphicsState& state, DeviceState& device, 
	VisualRenderInfo& vinfo, LightRenderInfo& linfo, PostProcessInfo& pinfo, 
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& lbuffer, ZBuffer& zbuffer, Buffer& backbuffer, Camera& camera)
{
	float const z_near = 0.1f;

	struct
	{
		uint width;
		uint height;
	} const settings = { 960, 960 };

	WNDCLASSEX window_class;
	ZeroMemory( &window_class, sizeof( WNDCLASSEX ) );
	window_class.lpszClassName = "Devora::window";                        
	window_class.cbSize = sizeof( WNDCLASSEX );      
	window_class.lpfnWndProc = WindowProc;  
	RegisterClassEx( &window_class );

	OK( device.window = CreateWindowEx( 0, window_class.lpszClassName, "Devora",
		WS_SYSMENU | WS_OVERLAPPED | WS_VISIBLE, CW_USEDEFAULT,
		CW_USEDEFAULT, settings.width, settings.height, NULL, NULL, NULL, 0 ) );

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

		camera.aperture = 1.0f;
		camera.field_of_view = 60;
		camera.aspect_ratio = float(settings.width) / settings.height;

		projection_matrix(vinfo.proj, camera.field_of_view, camera.aspect_ratio, z_near);
		projection_matrix(linfo.proj, 90, 1.0, z_near);
		pinfo.ambient = Vector3f(0.02f, 0.02f, 0.02f);
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
		desc.BufferDesc.Width = settings.width;
		desc.BufferDesc.Height = settings.height;
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
		IPtr<ID3D10Blob> code;
		IPtr<ID3D10Blob> info;
		IPtr<ID3DX11Effect> effect;

		uint flags = D3D10_SHADER_ENABLE_STRICTNESS |
		D3D10_SHADER_OPTIMIZATION_LEVEL0 |
		D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

		HOK_EX( D3DX11CompileFromFile( "shaders/Devora.hlsl",
		NULL, NULL, NULL, "fx_5_0", flags,
		0, NULL, &code, &info, NULL ), (char*)info->GetBufferPointer() );
	
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
		state.var.z_near = effect->GetVariableByName("z_near")->AsScalar();
		state.var.aspect_ratio = effect->GetVariableByName("aspect_ratio")->AsScalar();
		state.var.field_of_view = effect->GetVariableByName("field_of_view")->AsScalar();
		state.var.light_colour = effect->GetVariableByName("light_colour")->AsVector();
		state.var.light_pos = effect->GetVariableByName("light_pos")->AsVector();
		state.var.light_matrix = effect->GetVariableByName("light_matrix")->AsMatrix();
		state.var.view_i = effect->GetVariableByName("view_i")->AsMatrix();
		state.var.reproject = effect->GetVariableByName("reproject")->AsMatrix();
		state.var.cubeproj = effect->GetVariableByName("cubeproj")->AsMatrix();
		state.var.world_view = effect->GetVariableByName("world_view")->AsMatrix();
		state.var.world_view_proj = effect->GetVariableByName("world_view_proj")->AsMatrix();
		state.var.world_lightview_lightproj = effect->GetVariableByName("world_lightview_lightproj")->AsMatrix();
		state.var.world_lightview = effect->GetVariableByName("world_lightview")->AsMatrix();
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
		desc.Width = 512;
		desc.Height = 512; //!
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
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

		desc.Width = settings.width;
		desc.Height = settings.height;
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
		desc.Format = DXGI_FORMAT_R32_FLOAT;
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
		desc.Format = DXGI_FORMAT_D32_FLOAT;
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
		gbuffer0.viewport.Width = float(settings.width);
		gbuffer0.viewport.Height = float(settings.height);
		gbuffer0.viewport.MinDepth = 0.0f;
		gbuffer0.viewport.MaxDepth = 1.0f;
		gbuffer0.viewport.TopLeftX = 0.0f;
		gbuffer0.viewport.TopLeftY = 0.0f;

		backbuffer.viewport = lbuffer.viewport = zbuffer.viewport 
			= gbuffer1.viewport = gbuffer0.viewport;
	   
		shadowmap.viewport.Width = float(512);
		shadowmap.viewport.Height = float(512); //!
		shadowmap.viewport.MinDepth = 0.0f;
		shadowmap.viewport.MaxDepth = 1.0f;
		shadowmap.viewport.TopLeftX = 0.0f;
		shadowmap.viewport.TopLeftY = 0.0f;

		shadowcube.viewport = shadowmap.viewport;
	}

	//Geometry
	vinfo.geoms.push_back( read_geom(device, "geometry//plane.geom") );
	vinfo.geoms.push_back( read_geom(device, "geometry//icosphere.geom") );

	HOK( state.var.z_near->SetFloat( z_near ) );
}

} // namespace Devora
