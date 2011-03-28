#define D3D11_DEBUG_INFO

#define NOMINMAX

#include <d3dx11.h>
#include <d3dx10math.h>

#include "Ok.h"
#include "PixelShaderTracy.h"

typedef unsigned int uint;

using namespace common;

#define PI 3.1415926535897932384626433832795
const float to_radians = (float)(PI / 180.0);
const float from_radians = (float)(180.0 / PI);

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
			throw PixelShaderTracy::Quit();
    } 
    return DefWindowProc(handle, msg, w, l);
}

void PixelShaderTracy::on_key(int key, bool up)
{
	if(!up)
	{
		switch(key)
		{
		case 27: // ESC
			throw Quit();
			break;
		case 189: // -
			steps--;
			break;
		case 187: // +
			steps++;
			break;
		case 33: // Pg Up
			interp += 0.1f;
			if (interp > 1) interp = 1;
			break;
		case 34: // Pg Down
			interp -= 0.1f;
			if (interp < 0) interp = 0;
			break;
		}
	}
}

void PixelShaderTracy::on_mouse(long dx, long dy)
{
	camera.yaw += dx * settings.mouse_sens;
	camera.pitch += dy * settings.mouse_sens;
}

PixelShaderTracy::PixelShaderTracy(Settings settings_)
{
	settings = settings_;
	camera.yaw = camera.pitch = 0.0;

	// creating window
	{
		WNDCLASSEX window_class;
		ZeroMemory( &window_class, sizeof( WNDCLASSEX ) );
		window_class.lpszClassName = "Tracy::window";                        
		window_class.cbSize = sizeof( WNDCLASSEX );      
		window_class.lpfnWndProc = WindowProc;  
		RegisterClassEx( &window_class );

		OK_P( window_handle = CreateWindowEx( 0, "Tracy::window", "Tracy",
			WS_SYSMENU | WS_OVERLAPPED | WS_VISIBLE, CW_USEDEFAULT,
			CW_USEDEFAULT, settings.width, settings.height, NULL, NULL, NULL, 0 ) );
	}

	// 
	{
		iptr<IDXGIDevice1> dxgi_device;
		iptr<IDXGIAdapter1> dxgi_adapter;

		OK( D3D11CreateDevice
			( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG,
			NULL, 0, D3D11_SDK_VERSION, &device, NULL, &context ) );

		OK( device->QueryInterface
			( __uuidof(IDXGIDevice1), (void**)&dxgi_device ) );
    
		OK( dxgi_device->GetParent
			( __uuidof(IDXGIAdapter1), (void**)&dxgi_adapter ) );
    
		OK( dxgi_adapter->GetParent
			( __uuidof(IDXGIFactory1), (void**)&dxgi_factory ) );
	
		OK( dxgi_factory->MakeWindowAssociation(NULL, DXGI_MWA_NO_WINDOW_CHANGES) );
	}

	// swap chain
	{
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferCount = 2;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferDesc.Width = settings.width;
		desc.BufferDesc.Width = settings.height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.OutputWindow = window_handle;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = 1;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		OK( dxgi_factory->CreateSwapChain
			( device, &desc, &swap_chain ));
	}

	// target
	{
		iptr<ID3D11Texture2D> back_buffer;
	
		OK( swap_chain->GetBuffer
			( 0, __uuidof( ID3D11Texture2D ), ( void** )( &back_buffer ) ));

		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT(0);
		desc.Texture2D.MipSlice = 0;

		OK( device->CreateRenderTargetView
			( back_buffer, NULL, &target ));
	}

	// effect
	{
		iptr<ID3D10Blob> code;
		iptr<ID3D10Blob> info;

		OK_EX( D3DX11CompileFromFile( "shader.hlsl",
		NULL, NULL, NULL, "fx_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS |
		D3D10_SHADER_OPTIMIZATION_LEVEL0 |
		D3D10_SHADER_PACK_MATRIX_ROW_MAJOR |
		0, 0, NULL, &code, &info, NULL ), (char*)info->GetBufferPointer() );
	
		OK( D3DX11CreateEffectFromMemory( code->GetBufferPointer(),
			code->GetBufferSize(), 0, device, &effect ));

		pass = effect->GetTechniqueByIndex(0)->GetPassByIndex(0);
		var_view = effect->GetVariableByName("view")->AsMatrix();
		var_interp = effect->GetVariableByName("interp")->AsScalar();
		var_fov = effect->GetVariableByName("field_of_view")->AsScalar();
		var_aspect = effect->GetVariableByName("aspect_ratio")->AsScalar();
		var_steps = effect->GetVariableByName("steps")->AsScalar();
		var_steps->GetInt( &steps );
	}

	context->OMSetRenderTargets(1, &target, NULL);
	{
		D3D11_VIEWPORT viewport;
			
		viewport.Width = float(settings.width);
		viewport.Height = float(settings.height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
    
		context->RSSetViewports( 1, &viewport );
	}

	{
		iptr<ID3D11Buffer> vertex_buffer;

		{
			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.ByteWidth = 12 * sizeof(float);
	
			float buffer[6][2] = {
				{ 0.0f, 0.0f },
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },

				{ 1.0f, 1.0f },
				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f }  };

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = (void*)buffer;

			OK( device->CreateBuffer
			( &desc, &data, &vertex_buffer ) );
		}

		uint stride = 2 * sizeof(float);
		uint offset = 0;

		context->IASetPrimitiveTopology
		( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		context->IASetVertexBuffers
		( 0, 1, &vertex_buffer, 
			&stride, &offset );
	}

	{
		iptr<ID3D11InputLayout> input_layout;

		D3DX11_PASS_DESC desc;
		OK( pass->GetDesc( &desc ) );
        
		D3D11_INPUT_ELEMENT_DESC element =
		{
			"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,
			0, D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		};

		OK( device->CreateInputLayout
			( &element, 1, desc.pIAInputSignature,
				desc.IAInputSignatureSize, &input_layout ) );
                  
		context->IASetInputLayout( input_layout );
	}

	interp = 0.0;
	start_view = Matrix4f::Identity();

	OK( var_fov->SetFloat( 50.0 ) );
	OK( var_aspect->SetFloat( float(settings.width) / settings.height ) );
}

void PixelShaderTracy::step(DefaultInput& input)
{
	{
		const float white[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		context->ClearRenderTargetView(target, white);
	}

	for(auto i = input.keys.begin(); i != input.keys.end(); i++)
	{
		on_key( (*i).key, (*i).up );
	}

	on_mouse(input.mouse.x, input.mouse.y);
	
	{
		Matrix4f rot;
		D3DXMatrixRotationYawPitchRoll( reinterpret_cast<D3DXMATRIX*>(rot.data()), 
		camera.yaw*to_radians, camera.pitch*to_radians, 0.0);
		rot.transposeInPlace();
		view = start_view * rot;
	}
		
	OK( var_view->SetRawValue( (void*)view.data(), 0, sizeof(Matrix4f) ) );
	OK( var_interp->SetFloat( interp ) );
	OK( var_steps->SetInt( steps ) );
	OK( pass->Apply( 0, context ) );

	context->Draw( 6, 0 );

	OK( swap_chain->Present( 0, 0 ) );
}