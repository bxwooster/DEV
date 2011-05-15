#include "Camera.hpp"
#include "DeviceState.hpp"
#include "GraphicsState.hpp"
#include "Buffer.hpp"
#include "ZBuffer.hpp"
#include "Tools.hpp"

#include <D3DX11.h>

namespace DEV {

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

void InitGraphics(GraphicsState& state, DeviceState& device, 
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& lbuffer, ZBuffer& zbuffer, Buffer& backbuffer, Camera& camera)
{
	// Camera
	camera.z_near = 0.1f;
	int width = camera.screen.w = 960;
	int height = camera.screen.h = 960;
	camera.aspect_ratio = float(camera.screen.w) / camera.screen.h;

	int const shadowmap_size = 512;

	WNDCLASSEX window_class;
	ZeroMemory( &window_class, sizeof( WNDCLASSEX ) );
	window_class.lpszClassName = "DEV::window";                        
	window_class.cbSize = sizeof( WNDCLASSEX );      
	window_class.lpfnWndProc = WindowProc;  
	RegisterClassEx( &window_class );

	HWND window;
	OK( window = CreateWindowEx( 0, window_class.lpszClassName, "DEV",
		WS_SYSMENU | WS_OVERLAPPED | WS_VISIBLE, CW_USEDEFAULT,
		CW_USEDEFAULT, width, height, NULL, NULL, NULL, 0 ) );

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
			NULL, 0, D3D11_SDK_VERSION, ~device.device, &feature_level, ~state ) );

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
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.OutputWindow = window;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = 1;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HOK( dxgi_factory->CreateSwapChain
			( device.device, &desc, ~device.swap_chain ));
	}

	// Back buffer, view
	{
		HOK( device.swap_chain->GetBuffer
			( 0, __uuidof( ID3D11Texture2D ), ( void** )( &backbuffer.texture ) ));

		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT(0);
		desc.Texture2D.MipSlice = 0;

		HOK( device.device->CreateRenderTargetView
			( backbuffer.texture, NULL, ~backbuffer.rtv ));
	}

	// Textures
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc) );
		desc.Width = shadowmap_size;
		desc.Height = shadowmap_size;
		desc.Format = DXGI_FORMAT_R16_TYPELESS;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		desc.ArraySize = 6;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MipLevels = 1;
		desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

		HOK( device.device->CreateTexture2D( &desc, NULL, ~shadowcube.texture ) );
		desc.ArraySize = 1;
		desc.MiscFlags = 0;
		HOK( device.device->CreateTexture2D( &desc, NULL, ~shadowmap.texture ) );

		desc.Width = width;
		desc.Height = height;
		HOK( device.device->CreateTexture2D( &desc, NULL, ~zbuffer.texture ) );

		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		HOK( device.device->CreateTexture2D( &desc, NULL, ~gbuffer0.texture ) );
		HOK( device.device->CreateTexture2D( &desc, NULL, ~lbuffer.texture ) );

		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		HOK( device.device->CreateTexture2D( &desc, NULL, ~gbuffer1.texture ) );
	}

	// Views
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT_R16_UNORM;
		desc.Texture2D.MipLevels = 1;
		desc.Texture2D.MostDetailedMip = 0;

		HOK( device.device->CreateShaderResourceView
			( shadowmap.texture, &desc, ~shadowmap.srv ));

		HOK( device.device->CreateShaderResourceView
			( zbuffer.texture, &desc, ~zbuffer.srv ));

		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels = 1;
		desc.TextureCube.MostDetailedMip = 0;

		HOK( device.device->CreateShaderResourceView
		  ( shadowcube.texture, &desc, ~shadowcube.srv ));
	}
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT_D16_UNORM;
		desc.Texture2D.MipSlice = 0;

		HOK( device.device->CreateDepthStencilView
			( shadowmap.texture, &desc, ~shadowmap.dsv ));

		HOK( device.device->CreateDepthStencilView
		  ( zbuffer.texture, &desc, ~zbuffer.dsv ));

		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = 0;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.ArraySize = 6;

		HOK( device.device->CreateDepthStencilView
		  ( shadowcube.texture, &desc, ~shadowcube.dsv ));
	}
	HOK( device.device->CreateShaderResourceView
		( gbuffer0.texture, NULL, ~gbuffer0.srv ) );
	HOK( device.device->CreateShaderResourceView
		( gbuffer1.texture, NULL, ~gbuffer1.srv ) );
	HOK( device.device->CreateShaderResourceView
		( lbuffer.texture, NULL, ~lbuffer.srv ) );
	HOK( device.device->CreateRenderTargetView
		( gbuffer0.texture, NULL, ~gbuffer0.rtv ) );
	HOK( device.device->CreateRenderTargetView
		( gbuffer1.texture, NULL, ~gbuffer1.rtv ) );
	HOK( device.device->CreateRenderTargetView
		( lbuffer.texture, NULL, ~lbuffer.rtv ) );

	// Viewports
	{			
		gbuffer0.viewport.Width = float(width);
		gbuffer0.viewport.Height = float(height);
		gbuffer0.viewport.MinDepth = 0.0f;
		gbuffer0.viewport.MaxDepth = 1.0f;
		gbuffer0.viewport.TopLeftX = 0.0f;
		gbuffer0.viewport.TopLeftY = 0.0f;

		backbuffer.viewport = lbuffer.viewport = zbuffer.viewport 
			= gbuffer1.viewport = gbuffer0.viewport;
	   
		shadowmap.viewport.Width = float(shadowmap_size);
		shadowmap.viewport.Height = float(shadowmap_size);
		shadowmap.viewport.MinDepth = 0.0f;
		shadowmap.viewport.MaxDepth = 1.0f;
		shadowmap.viewport.TopLeftX = 0.0f;
		shadowmap.viewport.TopLeftY = 0.0f;

		shadowcube.viewport = shadowmap.viewport;
	}
}

} // namespace DEV
