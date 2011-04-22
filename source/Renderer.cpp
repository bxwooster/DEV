#define D3D11_DEBUG_INFO
#define NOMINMAX

#include <iostream>
#include <fstream>
#include <d3dx11.h>
#include <d3dx10math.h>
#include "Renderer.h"

#define PI 3.1415926535897932384626433832795
const float to_radians = (float)(PI / 180.0);
const float from_radians = (float)(180.0 / PI);

void projection_matrix(Matrix4f& proj, float y_fov, float aspect_ratio, float z_near)
{
	float ys = 1 / tanf( y_fov * to_radians / 2.0f );
	float xs = ys / aspect_ratio;
	proj << xs, 0, 0, 0,
			0, ys, 0, 0,
			0, 0, -1, -z_near,
			0, 0, -1, 0;
}

Renderer::Renderer(ObjectData& object_, LightData& light_, Settings settings_) :
	settings(settings_),
	window(settings.width, settings.height),
	object(object_),
	light(light_)
{
	// Misc
	{
		z_near = 0.2f;
		camera.yaw = camera.pitch = 0.0;
		aperture = 1.0f;
		eye = Vector3f(5, 0, 5);
		ambient = Vector3f(0.05f, 0.04f, 0.05f);

		field_of_view = 60;
		aspect_ratio = float(settings.width) / settings.height;

		projection_matrix(proj, field_of_view, aspect_ratio, z_near);
		projection_matrix(lightproj, 90, 1.0, z_near);

		view_axis << 0.0, 1.0, 0.0, 0.0,
					 0.0, 0.0, 1.0, 0.0,
					 1.0, 0.0, 0.0, 0.0,
					 0.0, 0.0, 0.0, 1.0;
	}

	// Device, Factory
	{
		iptr<IDXGIDevice1> dxgi_device;
		iptr<IDXGIAdapter1> dxgi_adapter;

		D3D_FEATURE_LEVEL feature_level;

#ifdef DEBUG //!
		D3D10_CREATE_DEVICE_FLAG flag = D3D10_CREATE_DEVICE_DEBUG;
#else
		D3D10_CREATE_DEVICE_FLAG flag = D3D10_CREATE_DEVICE_FLAG(0);
#endif
		OK( D3D11CreateDevice
			( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flag,
			NULL, 0, D3D11_SDK_VERSION, &device, &feature_level, &context ) );

		OK( device->QueryInterface
			( __uuidof(IDXGIDevice1), (void**)&dxgi_device ) );
    
		OK( dxgi_device->GetParent
			( __uuidof(IDXGIAdapter1), (void**)&dxgi_adapter ) );
    
		OK( dxgi_adapter->GetParent
			( __uuidof(IDXGIFactory1), (void**)&dxgi_factory ) );
	
		OK( dxgi_factory->MakeWindowAssociation(NULL, DXGI_MWA_NO_WINDOW_CHANGES) );
	}

	// Swap chain
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

	// Back buffer, view
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

	// Effect, Variables, Passes
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
		pass.render_z = effect->GetTechniqueByName("render_z")->GetPassByIndex(0);
		pass.render_cube_z = effect->GetTechniqueByName("render_cube_z")->GetPassByIndex(0);
		pass.directional_light = effect->GetTechniqueByName("directional_light")->GetPassByIndex(0);
		pass.point_light = effect->GetTechniqueByName("point_light")->GetPassByIndex(0);
		pass.ambient_light = effect->GetTechniqueByName("ambient_light")->GetPassByIndex(0);
		pass.sky = effect->GetTechniqueByName("sky")->GetPassByIndex(0);
		pass.hdr = effect->GetTechniqueByName("hdr")->GetPassByIndex(0);
		pass.test = effect->GetTechniqueByName("test")->GetPassByIndex(0);

		var.accum = effect->GetVariableByName("accum")->AsShaderResource();
		var.zbuffer = effect->GetVariableByName("zbuffer")->AsShaderResource();
		var.shadowmap = effect->GetVariableByName("shadowmap")->AsShaderResource();
		var.shadowcube = effect->GetVariableByName("shadowcube")->AsShaderResource();
		var.gbuffer0 = effect->GetVariableByName("gbuffer0")->AsShaderResource();
		var.gbuffer1 = effect->GetVariableByName("gbuffer1")->AsShaderResource();
		var.aperture = effect->GetVariableByName("aperture")->AsScalar();
		var.z_near = effect->GetVariableByName("z_near")->AsScalar();
		var.aspect_ratio = effect->GetVariableByName("aspect_ratio")->AsScalar();
		var.field_of_view = effect->GetVariableByName("field_of_view")->AsScalar();
		var.light_colour = effect->GetVariableByName("light_colour")->AsVector();
		var.light_pos = effect->GetVariableByName("light_pos")->AsVector();
		var.view_proj = effect->GetVariableByName("view_proj")->AsMatrix();
		var.view_i = effect->GetVariableByName("view_i")->AsMatrix();
		var.view = effect->GetVariableByName("view")->AsMatrix();
		var.reproject = effect->GetVariableByName("reproject")->AsMatrix();
		var.cubeproj = effect->GetVariableByName("cubeproj")->AsMatrix();
		var.world_view = effect->GetVariableByName("world_view")->AsMatrix();
		var.world_view_proj = effect->GetVariableByName("world_view_proj")->AsMatrix();
		var.world_lightview_lightproj = effect->GetVariableByName("world_lightview_lightproj")->AsMatrix();
		var.world_lightview = effect->GetVariableByName("world_lightview")->AsMatrix();
	}

	// Quad
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

	// Layouts
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

		OK( device->CreateTexture2D( &desc, NULL, &shadowcube ) );
		desc.ArraySize = 1;
		OK( device->CreateTexture2D( &desc, NULL, &shadowmap ) );

		desc.Width = settings.width;
		desc.Height = settings.height;
		OK( device->CreateTexture2D( &desc, NULL, &zbuffer ) );

		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		OK( device->CreateTexture2D( &desc, NULL, &gbuffer0 ) );
		OK( device->CreateTexture2D( &desc, NULL, &accum ) );

		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		OK( device->CreateTexture2D( &desc, NULL, &gbuffer1 ) );
	}

	// Views
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.Texture2D.MipLevels = 1;
		desc.Texture2D.MostDetailedMip = 0;

		OK( device->CreateShaderResourceView
		  ( shadowmap, &desc, &shadowmap_srv ));

		OK( device->CreateShaderResourceView
		  ( zbuffer, &desc, &zbuffer_srv ));

		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.MostDetailedMip = 0;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.ArraySize = 6;

		OK( device->CreateShaderResourceView
		  ( shadowcube, &desc, &shadowcube_srv ));
	}
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT_D32_FLOAT;
		desc.Texture2D.MipSlice = 0;

		OK( device->CreateDepthStencilView
		  ( shadowmap, &desc, &shadowmap_dsv ));

		OK( device->CreateDepthStencilView
		  ( zbuffer, &desc, &zbuffer_dsv ));

		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = 0;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.ArraySize = 6;

		OK( device->CreateDepthStencilView
		  ( shadowcube, &desc, &shadowcube_dsv ));
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

	// Viewports
	{			
		viewport_screen.Width = float(settings.width);
		viewport_screen.Height = float(settings.height);
		viewport_screen.MinDepth = 0.0f;
		viewport_screen.MaxDepth = 1.0f;
		viewport_screen.TopLeftX = 0.0f;
		viewport_screen.TopLeftY = 0.0f;
	   
		viewport_shadowmap.Width = float(512);
		viewport_shadowmap.Height = float(512); //!
		viewport_shadowmap.MinDepth = 0.0f;
		viewport_shadowmap.MaxDepth = 1.0f;
		viewport_shadowmap.TopLeftX = 0.0f;
		viewport_shadowmap.TopLeftY = 0.0f;
	}
}

void Renderer::render()
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
		context->ClearDepthStencilView(zbuffer_dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
	}
	
	Matrix4f rot;
	D3DXMatrixRotationYawPitchRoll
		( (D3DXMATRIX*) rot.data(), 
		camera.pitch * to_radians, 0.0, camera.yaw * to_radians);
	rot.transposeInPlace();


	Matrix4f translate = Matrix4f::Identity();
	translate.col(3) << -eye, 1.0;
	Matrix4f view = view_axis * rot * translate;
	
	Matrix4f view_i( view.inverse() );
	OK( var.view_i->SetMatrix( view_i.data() ));
	OK( var.view->SetMatrix( view.data() ));

	Matrix4f view_proj( proj * view );
	OK( var.view_proj->SetMatrix( view_proj.data() ));

	Vector3f colour(0.1f, 0.1f, 0.1f);
	OK( var.light_colour->SetRawValue
		( (void*)colour.data(), 0, sizeof(Vector3f) ) );

	OK( var.aperture->SetFloat( aperture ) );
	OK( var.z_near->SetFloat( z_near ) );
	OK( var.field_of_view->SetFloat( field_of_view ) );
	OK( var.aspect_ratio->SetFloat( aspect_ratio ) );

	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// step 1: render
	ID3D11RenderTargetView* targets[] = { gbuffer0_rtv, gbuffer1_rtv };
	context->OMSetRenderTargets(2, targets, zbuffer_dsv);
	context->RSSetViewports( 1, &viewport_screen );
	context->IASetInputLayout( input_layout_objects );

	for (int i = 0; i < object.transforms.size(); i++)
	{
		Matrix4f world_view = view * object.transforms[i];
		Matrix4f world_view_proj = proj * world_view;
	
		OK( var.world_view->SetMatrix( world_view.data() ));
		OK( var.world_view_proj->SetMatrix( world_view_proj.data() ));
		OK( pass.render->Apply( 0, context ) );
		
		context->IASetVertexBuffers
			(0, 1, &object.geometries[i].buffer, &object.geometries[i].stride, &object.geometries[i].offset);
		
		context->Draw( object.geometries[i].count, 0 );
	}

	// step 2: lights
	OK( var.gbuffer0->SetResource( gbuffer0_srv ) );
	OK( var.gbuffer1->SetResource( gbuffer1_srv ) );
	OK( var.zbuffer->SetResource( zbuffer_srv ) );

	for (int k = 0; k < light.transforms.size(); k++)
	{
		context->IASetInputLayout( input_layout_objects );

		Matrix4f lightview = view_axis * light.transforms[k].inverse();
		Matrix4f lightview_lightproj = lightproj * lightview;

		if (light.types[k] == LightType_directional)
		{
			context->ClearDepthStencilView
				(shadowmap_dsv, D3D11_CLEAR_DEPTH, 1.0, 0);

			context->OMSetRenderTargets(0, NULL, shadowmap_dsv);
			context->RSSetViewports( 1, &viewport_shadowmap );

			Matrix4f reproject = lightview_lightproj * view.inverse();
			OK( var.reproject->SetMatrix( reproject.data() ));

			for (int i = 0; i < object.transforms.size(); i++)
			{
				Matrix4f world_lightview_lightproj = lightview_lightproj * object.transforms[i];
				OK( var.world_lightview_lightproj->SetMatrix( world_lightview_lightproj.data() ));
				OK( pass.render_z->Apply( 0, context ) );
		
				context->IASetVertexBuffers
					(0, 1, &object.geometries[i].buffer, &object.geometries[i].stride, &object.geometries[i].offset);
				context->Draw( object.geometries[i].count, 0 );
			}
		}
		else if (light.types[k] == LightType_point)
		{
			context->ClearDepthStencilView
				(shadowcube_dsv, D3D11_CLEAR_DEPTH, 1.0, 0);

			context->OMSetRenderTargets(0, NULL, shadowcube_dsv);
			auto v = viewport_shadowmap;
			D3D11_VIEWPORT viewports[6] = {v, v, v, v, v, v};
			context->RSSetViewports( 6, viewports );

			for (int i = 0; i < object.transforms.size(); i++)
			{
				Matrix4f world_lightview = lightview * object.transforms[i];
				OK( var.world_lightview->SetMatrix( world_lightview.data() ));
				Matrix4f m = Matrix4f::Identity();
				Matrix4f cubeproj[6] = {m, m, m, m, m, m}; //!
				OK( var.cubeproj->SetMatrixArray(cubeproj[0].data(), 0, 6));
				OK( pass.render_cube_z->Apply( 0, context ) );
		
				context->IASetVertexBuffers
					(0, 1, &object.geometries[i].buffer, &object.geometries[i].stride, &object.geometries[i].offset);
				context->Draw( object.geometries[i].count, 0 );
			}
		}

		context->IASetInputLayout( input_layout_quad );
		context->RSSetViewports( 1, &viewport_screen );
		context->IASetVertexBuffers(0, 1, &quad.buffer, &quad.stride, &quad.offset);
		context->OMSetRenderTargets(1, &accum_rtv, NULL);

		Vector4f position(view * light.transforms[k].col(3));
		OK( var.light_pos->SetRawValue
			( (void*)position.data(), 0, sizeof(Vector3f) ) );

		Vector3f& colour = light.colours[k];
		OK( var.light_colour->SetRawValue
			( (void*)colour.data(), 0, sizeof(Vector3f) ) );
		
		OK( var.shadowmap->SetResource( shadowmap_srv ) );
		OK( var.shadowcube->SetResource( shadowcube_srv ) );
		if (light.types[k] == LightType_directional)
			{OK( pass.directional_light->Apply( 0, context ) );}
		else if (light.types[k] == LightType_point)
			{OK( pass.point_light->Apply( 0, context ) );}
		context->Draw( quad.count, 0 );
	}

	// step 3: ambient
	OK( var.light_colour->SetRawValue
		( (void*)ambient.data(), 0, sizeof(Vector3f) ) );
	OK( pass.ambient_light->Apply( 0, context ) );
	context->Draw( quad.count, 0 );

	// step 4: sky
	context->OMSetRenderTargets(1, &accum_rtv, zbuffer_dsv);
	OK( pass.sky->Apply( 0, context ) );
	context->Draw( quad.count, 0 );

	// step 5: "HDR"
	OK( var.accum->SetResource( accum_srv ) );
	//context->IASetInputLayout( NULL );
	//context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );	
	context->OMSetRenderTargets(1, &window_rtv, NULL);
	OK( pass.hdr->Apply( 0, context ) );
	context->Draw( 1, 0 );
	
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