#define NOMINMAX
#define D3D11_DEBUG_INFO

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx11effect.h>
#include <d3dx10math.h>

#include <iostream>

#include "math.h"
#include "matrix.h"
#include "ok.h"
#include "iptr.h"

using namespace common;
typedef unsigned int uint;

class Quit {};

struct
{
	HWND window_handle;
	iptr<ID3D11Device> device;
	iptr<ID3D11DeviceContext> context;
	iptr<IDXGIFactory1> dxgi_factory;
	iptr<IDXGISwapChain> swap_chain;
	iptr<ID3D11RenderTargetView> target;
	iptr<ID3DX11Effect> effect;
	ID3DX11EffectPass* pass;
	ID3DX11EffectVectorVariable* var_eye;
	ID3DX11EffectMatrixVariable* var_view;
	ID3DX11EffectMatrixVariable* var_view_inv;
	ID3DX11EffectScalarVariable* var_interp0;
	ID3DX11EffectScalarVariable* var_interp1;
	ID3DX11EffectScalarVariable* var_fov;
	ID3DX11EffectScalarVariable* var_aspect;
	ID3DX11EffectScalarVariable* var_steps;

	Matrix4f start_view;
	Matrix4f view;
	float interp0, interp1;
	Vector4f eye;
	int steps;
} g;

struct
{
	int width;
	int height;
	float mouse_sens;
} g_settings;

struct
{
	float yaw;
	float pitch;
} g_camera;

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
    } 
    return DefWindowProc(handle, msg, w, l);
}

void init()
{
	{
	    RAWINPUTDEVICE info[2];
		info[0].dwFlags = 0;
		info[0].hwndTarget = NULL;
		info[0].usUsagePage = 1;
		info[0].usUsage = 6; //Keyboard
    
		info[1].dwFlags = 0;
		info[1].hwndTarget = NULL;
		info[1].usUsagePage = 1;
		info[1].usUsage = 2; //Mouse
    
		RegisterRawInputDevices( info, 2, sizeof(RAWINPUTDEVICE) );
	}

	// creating window
	{
		WNDCLASSEX window_class;
		ZeroMemory( &window_class, sizeof( WNDCLASSEX ) );
		window_class.lpszClassName = "Tracy::window";                        
		window_class.cbSize = sizeof( WNDCLASSEX );      
		window_class.lpfnWndProc = WindowProc;  
		OK_P( RegisterClassEx( &window_class ));

		OK_P( g.window_handle = CreateWindowEx( 0, "Tracy::window", "Tracy",
			WS_SYSMENU | WS_OVERLAPPED | WS_VISIBLE, CW_USEDEFAULT,
			CW_USEDEFAULT, g_settings.width, g_settings.height, NULL, NULL, NULL, 0 ) );
	}

	// 
	{
		iptr<IDXGIDevice1> dxgi_device;
		iptr<IDXGIAdapter1> dxgi_adapter;

		OK( D3D11CreateDevice
			( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG,
			NULL, 0, D3D11_SDK_VERSION, &g.device, NULL, &g.context ) );
	    
		OK( g.device->QueryInterface
		  ( __uuidof(IDXGIDevice1), (void**)&dxgi_device ) );
    
		OK( dxgi_device->GetParent
		  ( __uuidof(IDXGIAdapter1), (void**)&dxgi_adapter ) );
    
		OK( dxgi_adapter->GetParent
		  ( __uuidof(IDXGIFactory1), (void**)&g.dxgi_factory ) );
	
		OK( g.dxgi_factory->MakeWindowAssociation(NULL, DXGI_MWA_NO_WINDOW_CHANGES) );
	}

	// swap chain
	{
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferCount = 2;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferDesc.Width = g_settings.width;
		desc.BufferDesc.Width = g_settings.height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.OutputWindow = g.window_handle;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = 1;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		OK( g.dxgi_factory->CreateSwapChain
		  ( g.device, &desc, &g.swap_chain ));
	}

	// target
	{
		iptr<ID3D11Texture2D> back_buffer;
	
		OK( g.swap_chain->GetBuffer
		  ( 0, __uuidof( ID3D11Texture2D ), ( void** )( &back_buffer ) ));

		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT(0);
		desc.Texture2D.MipSlice = 0;

		OK( g.device->CreateRenderTargetView
		  ( back_buffer, NULL, &g.target ));
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
			code->GetBufferSize(), 0, g.device, &g.effect ));

		g.pass = g.effect->GetTechniqueByIndex(0)->GetPassByIndex(0);
		g.var_eye = g.effect->GetVariableByName("eye")->AsVector();
		g.var_view = g.effect->GetVariableByName("view")->AsMatrix();
		g.var_view_inv = g.effect->GetVariableByName("view_inv")->AsMatrix();
		g.var_interp0 = g.effect->GetVariableByName("interp0")->AsScalar();
		g.var_interp1 = g.effect->GetVariableByName("interp1")->AsScalar();
		g.var_fov = g.effect->GetVariableByName("field_of_view")->AsScalar();
		g.var_aspect = g.effect->GetVariableByName("aspect_ratio")->AsScalar();
		g.var_steps = g.effect->GetVariableByName("steps")->AsScalar();
		g.var_steps->GetInt( &g.steps );
	}

	g.context->OMSetRenderTargets(1, &g.target, NULL);
	{
		D3D11_VIEWPORT viewport;
			
		viewport.Width = float(g_settings.width);
		viewport.Height = float(g_settings.height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
    
		g.context->RSSetViewports( 1, &viewport );
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

			OK( g.device->CreateBuffer
			( &desc, &data, &vertex_buffer ) );
		}

		uint stride = 2 * sizeof(float);
		uint offset = 0;

		g.context->IASetPrimitiveTopology
		( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		g.context->IASetVertexBuffers
		( 0, 1, &vertex_buffer, 
			&stride, &offset );
	}

	{
		iptr<ID3D11InputLayout> input_layout;

		D3DX11_PASS_DESC desc;
		OK( g.pass->GetDesc( &desc ) );
        
		D3D11_INPUT_ELEMENT_DESC element =
		{
			"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,
			0, D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		};

		OK( g.device->CreateInputLayout
			( &element, 1, desc.pIAInputSignature,
				desc.IAInputSignatureSize, &input_layout ) );
                  
		g.context->IASetInputLayout( input_layout );
	}

	g.interp0 = g.interp1 = 0;
	g.eye = Vector4f::UnitX() * 5.0;
	
	{
		D3DXVECTOR3 to(0.0, 0.0, 0.0);
		D3DXVECTOR3 at(-1.0, 0.0, 0.0);
		D3DXVECTOR3 up(0.0, 0.0, 1.0);
		D3DXMatrixLookAtRH( (D3DXMATRIX*)(g.start_view.data()), &to, &at, &up );
		g.start_view.transposeInPlace();
		g.start_view = Matrix4f::Identity();
		g.view = g.start_view;
	}

	g.var_fov->SetFloat( 50.0 );
	g.var_aspect->SetFloat( float(g_settings.width) / g_settings.height );
}

void key(int key, bool up)
{
	if(!up)
	{
		switch(key)
		{
		case 27: // ESC
			throw Quit();
			break;
		case 189: // -
			g.steps--;
			break;
		case 187: // +
			g.steps++;
			break;
		case 33: // Pg Up
			g.interp0 += 0.1f;
			if (g.interp0 > 1) g.interp0 = 1;
			break;
		case 34: // Pg Down
			g.interp0 -= 0.1f;
			if (g.interp0 < 0) g.interp0 = 0;
			break;
		}
	}
}

void mouse(long dx, long dy)
{
	g_camera.yaw += dx * g_settings.mouse_sens;
	g_camera.pitch += dy * g_settings.mouse_sens;

	Matrix4f rot;
	D3DXMatrixRotationYawPitchRoll( reinterpret_cast<D3DXMATRIX*>(rot.data()), 
	g_camera.yaw*to_radians, g_camera.pitch*to_radians, 0.0);
	rot.transposeInPlace();
	
	g.view = rot * g.start_view;
}

void run()
{
	while(true)
	{
		{
			MSG m;
			while (PeekMessage(&m, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&m);
				DispatchMessage(&m); // this calls window procs!
        
				switch( m.message )
				{
				case WM_INPUT:
					uint size;
					GetRawInputData((HRAWINPUT)m.lParam, 
						RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
                   
					byte* bytes = new byte[size];
					GetRawInputData((HRAWINPUT)m.lParam, 
						RID_INPUT, bytes, &size, sizeof(RAWINPUTHEADER));

					RAWINPUT* raw = (RAWINPUT*)bytes;
					if ( raw->header.dwType == RIM_TYPEKEYBOARD )
					{
						RAWKEYBOARD* data = &raw->data.keyboard;
						bool up = data->Flags & RI_KEY_BREAK;
						key( data->VKey, up );
					}
					else if ( raw->header.dwType == RIM_TYPEMOUSE )
					{
						RAWMOUSE* data = &raw->data.mouse;
						mouse( data->lLastX, data->lLastY );
					}
				
					delete[] bytes;
					break;
				}
			}
		}

		{
			const float white[4] = {1.0f, 1.0f, 1.0f, 0.0f};
			g.context->ClearRenderTargetView(g.target, white);
		}
		
		g.var_view->SetRawValue( (void*)g.view.data(), 0, sizeof(Matrix4f) );
		g.var_view_inv->SetRawValue( (void*)g.view.inverse().data(), 0, sizeof(g.view) );
		g.var_eye->SetRawValue( (void*)g.eye.data(), 0, sizeof(g.eye) );
		g.var_interp0->SetFloat( g.interp0 );
		g.var_interp1->SetFloat( g.interp1 );
		g.var_steps->SetInt( g.steps );
		OK( g.pass->Apply( 0, g.context ) );

		g.context->Draw( 6, 0 );
		OK( g.swap_chain->Present( 0, 0 ) );
	}
}

int main()
{
	g_settings.width = 960;
	g_settings.height = 960;
	g_settings.mouse_sens = 0.25;

	try
	{
		init();
		run();
	}
	catch(std::exception exception)
	{
		std::cout << exception.what();
		std::getchar();
		return -1;
	}
	catch(Quit) 
	{
		return 0;
	}
}