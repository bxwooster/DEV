#define D3D11_DEBUG_INFO
#define NOMINMAX

#include <fstream>
#include <d3dx11.h>
#include <d3dx10math.h>

#include "Renderer.h"

#define PI 3.1415926535897932384626433832795
const float to_radians = (float)(PI / 180.0);
const float from_radians = (float)(180.0 / PI);

Renderer::Renderer(Settings settings_) :
	settings(settings_),
	window(settings.width, settings.height)
{
	camera.yaw = camera.pitch = 0.0;
	// 
	{
		iptr<IDXGIDevice1> dxgi_device;
		iptr<IDXGIAdapter1> dxgi_adapter;

		D3D_FEATURE_LEVEL feature_level;

		OK( D3D11CreateDevice
			( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG,
			NULL, 0, D3D11_SDK_VERSION, &device, &feature_level, &context ) );

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
		desc.BufferDesc.Height = settings.height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.OutputWindow = window.handle;
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
			( back_buffer, NULL, &window_rtv ));
	}

	// effect
	{
		iptr<ID3D10Blob> code;
		iptr<ID3D10Blob> info;

		OK_EX( D3DX11CompileFromFile( "Freddy.hlsl",
		NULL, NULL, NULL, "fx_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS |
		D3D10_SHADER_OPTIMIZATION_LEVEL0 |
		D3D10_SHADER_PACK_MATRIX_ROW_MAJOR |
		0, 0, NULL, &code, &info, NULL ), (char*)info->GetBufferPointer() );
	
		OK( D3DX11CreateEffectFromMemory( code->GetBufferPointer(),
			code->GetBufferSize(), 0, device, &effect ));

		pass.render = effect->GetTechniqueByName("render")->GetPassByIndex(0);
		pass.directional_light = effect->GetTechniqueByName("directional_light")->GetPassByIndex(0);
		pass.ambient_light = effect->GetTechniqueByName("ambient_light")->GetPassByIndex(0);
		pass.sky = effect->GetTechniqueByName("sky")->GetPassByIndex(0);
		pass.hdr = effect->GetTechniqueByName("hdr")->GetPassByIndex(0);

		var.accum = effect->GetVariableByName("accum")->AsShaderResource();
		var.zbuffer = effect->GetVariableByName("zbuffer")->AsShaderResource();
		var.gbuffer0 = effect->GetVariableByName("gbuffer0")->AsShaderResource();
		var.gbuffer1 = effect->GetVariableByName("gbuffer1")->AsShaderResource();
		var.aperture = effect->GetVariableByName("aperture")->AsScalar();
		var.aspect_ratio = effect->GetVariableByName("aspect_ratio")->AsScalar();
		var.field_of_view = effect->GetVariableByName("field_of_view")->AsScalar();
		var.light_colour = effect->GetVariableByName("light_colour")->AsVector();
		var.light_pos = effect->GetVariableByName("light_pos")->AsVector();
		var.mViewProj = effect->GetVariableByName("mViewProj")->AsMatrix();
		var.mView_I = effect->GetVariableByName("mView_I")->AsMatrix();
		var.mWorldView = effect->GetVariableByName("mWorldView")->AsMatrix();
		var.mWorldViewProj = effect->GetVariableByName("mWorldViewProj")->AsMatrix();
	}

	{
		const uint size = 2;
		const uint offset = 0;
		const uint count = 6;

		iptr<ID3D11Buffer> vertex_buffer;

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = size * count * sizeof(float);
	
		float buffer[count][size] = {
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

		Geometry quad_ = { vertex_buffer, size * sizeof(float), offset, count };
		quad = quad_;
	}

	{
		D3DX11_PASS_DESC desc;
		OK( pass.sky->GetDesc( &desc ) );
        
		D3D11_INPUT_ELEMENT_DESC element =
		{
			"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,
			0, D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		};

		OK( device->CreateInputLayout
			( &element, 1, desc.pIAInputSignature,
				desc.IAInputSignatureSize, &input_layout_quad ) );
	}

	{
		D3DX11_PASS_DESC desc;
		OK( pass.render->GetDesc( &desc ) );
        
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

		OK( device->CreateInputLayout
			( element, 2, desc.pIAInputSignature,
				desc.IAInputSignatureSize, &input_layout_objects ) );
	}

	{
		float field_of_view = 90.0;
		float aspect_ratio = float(settings.width) / settings.height;

		float zn = 0.01f;
		float eps = 3e-7f;
		//float zf = 1000.0f;
		float y_scale = 1 / tanf( field_of_view * to_radians / 2.0f );
		float x_scale = y_scale / aspect_ratio;
		float c = eps - 1;//zf / (zn - zf);
		float d = zn * c;
		proj << x_scale, 0, 0, 0,
					0, y_scale, 0, 0,
					0, 0, c, d,
					0, 0, -1, 0;

		start_view << 0.0, 1.0, 0.0, 0.0,
					  0.0, 0.0, 1.0, -1.0,
					  1.0, 0.0, 0.0, -4.0,
					 0.0, 0.0, 0.0, 1.0;

		OK( var.field_of_view->SetFloat( field_of_view ) );
		OK( var.aspect_ratio->SetFloat( aspect_ratio ) );
	}
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc) );
		desc.Width = settings.width;
		desc.Height = settings.height;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MipLevels = 1;

		OK( device->CreateTexture2D( &desc, NULL, &zbuffer ) );

		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		OK( device->CreateTexture2D( &desc, NULL, &gbuffer0 ) );
		OK( device->CreateTexture2D( &desc, NULL, &accum ) );

		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		OK( device->CreateTexture2D( &desc, NULL, &gbuffer1 ) );
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.Texture2D.MipLevels = 1;

		OK( device->CreateShaderResourceView
		  ( zbuffer, &desc, &zbuffer_srv ));
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.Texture2D.MipSlice = 0;

		OK( device->CreateDepthStencilView
		  ( zbuffer, &desc, &zbuffer_dsv ));
	}

	OK( device->CreateShaderResourceView
		( gbuffer0, NULL, &gbuffer0_srv ) );
	OK( device->CreateShaderResourceView
		( gbuffer1, NULL, &gbuffer1_srv ) );
	OK( device->CreateShaderResourceView
		( accum, NULL, &accum_srv ) );
	
	OK( device->CreateRenderTargetView
		( gbuffer0, NULL, &gbuffer0_rtv ) );
	OK( device->CreateRenderTargetView
		( gbuffer1, NULL, &gbuffer1_rtv ) );
	OK( device->CreateRenderTargetView
		( accum, NULL, &accum_rtv ) );

	aperture = 1.0;
}

void Renderer::render(
	std::vector<Object, Eigen::aligned_allocator<Object>>& objects,
	std::vector<Light, Eigen::aligned_allocator<Light>>& lights )
{
	//clear
	{
		context->ClearState();

		const float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		const float white[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		context->ClearRenderTargetView(window_rtv, black);
		context->ClearRenderTargetView(gbuffer0_rtv, black);
		context->ClearRenderTargetView(gbuffer1_rtv, black);
		context->ClearRenderTargetView(accum_rtv, black);
		context->ClearDepthStencilView
			(zbuffer_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
	}
	
	Matrix4f rot1, rot2;
	D3DXMatrixRotationYawPitchRoll
		( reinterpret_cast<D3DXMATRIX*>(rot1.data()), 
		camera.yaw * to_radians, 0.0, 0.0);
	D3DXMatrixRotationYawPitchRoll
		( reinterpret_cast<D3DXMATRIX*>(rot2.data()), 
		0.0, camera.pitch * to_radians, 0.0);
	Matrix4f view = rot2.transpose() * rot1.transpose() * start_view;
	
	Matrix4f view_I( view.inverse() );
	OK( var.mView_I->SetMatrix( view_I.data() ));

	Matrix4f viewProj( proj * view );
	OK( var.mViewProj->SetMatrix( viewProj.data() ));

	Vector3f colour(0.1f, 0.1f, 0.1f);
	OK( var.light_colour->SetRawValue
		( (void*)colour.data(), 0, sizeof(Vector3f) ) );

	OK( var.aperture->SetFloat( aperture ) );

	//viewport
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
	//ia
	{
		context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		context->IASetInputLayout( input_layout_objects );
	}

	// step 1: render
	ID3D11RenderTargetView* targets[] = { gbuffer0_rtv, gbuffer1_rtv };
	context->OMSetRenderTargets(2, targets, zbuffer_dsv);


	for (auto i = objects.begin(); i != objects.end(); i++)
	{
		Object& object = *i;
		Matrix4f worldView = view * object.world;
		Matrix4f worldViewProj = viewProj * object.world;
	
		OK( var.mWorldView->SetMatrix( worldView.data() ));

		OK( var.mWorldViewProj->SetMatrix( worldViewProj.data() ));

		OK( pass.render->Apply( 0, context ) );
		
		context->IASetVertexBuffers
			(0, 1, &object.geometry.buffer, &object.geometry.stride, &object.geometry.offset);
		
		context->Draw( object.geometry.count, 0 );
	}

	// step 2: lights
	context->IASetInputLayout( input_layout_quad );
	context->IASetVertexBuffers(0, 1, &quad.buffer, &quad.stride, &quad.offset);
	
	context->OMSetRenderTargets(1, &accum_rtv, NULL);
	
	OK( var.gbuffer0->SetResource( gbuffer0_srv ) );
	OK( var.gbuffer1->SetResource( gbuffer1_srv ) );
	OK( var.zbuffer->SetResource( zbuffer_srv ) );

	for (auto i = lights.begin(); i != lights.end(); i++)
	{
		Vector4f position( (*i).world.col(3) );
		OK( var.light_pos->SetRawValue
			( (void*)position.data(), 0, sizeof(Vector3f) ) );

		Vector3f& colour = (*i).colour;
		OK( var.light_colour->SetRawValue
			( (void*) colour.data(), 0, sizeof(Vector3f) ) );

		OK( pass.directional_light->Apply( 0, context ) );
		context->Draw( quad.count, 0 );
	}

	// step 3: ambient
	OK( pass.ambient_light->Apply( 0, context ) );
	//context->Draw( quad.count, 0 );

	// step 4: sky
	context->OMSetRenderTargets(1, &accum_rtv, zbuffer_dsv);
	OK( pass.sky->Apply( 0, context ) );
	context->Draw( quad.count, 0 );

	// step 5: "HDR"
	OK( var.accum->SetResource( accum_srv ) );
	context->OMSetRenderTargets(1, &window_rtv, NULL);
	OK( pass.hdr->Apply( 0, context ) );
	context->Draw( quad.count, 0 );
	
	// finished
	OK( swap_chain->Present( 0, 0 ) );
};

Renderer::Geometry Renderer::read_geom(const std::string& filename)
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

    OK( device->CreateBuffer
      ( &buffer_desc, &data, &geometry.buffer ) );

	delete[] vertices;

	return geometry;
}