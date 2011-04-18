#ifndef __Renderer_h__
#define __Renderer_h__

#include "Window.h"
#include "Matrix.h"
#include "iPtr.h"

#include <d3d11.h>
#include <d3dx11effect.h>

typedef unsigned int uint;

using namespace common;

class Renderer
{
public:
	struct Settings
	{
		int width;
		int height;
	};

	struct Geometry
	{
		iptr<ID3D11Buffer> buffer;
		uint stride;
		uint offset;
		uint count;
	};

	struct Object
	{
		Matrix4f world;
		Geometry geometry;
	};

	struct Light
	{
		Matrix4f world;
		Vector3f colour;
	};
	
private:
	iptr<ID3D11Device> device;
	iptr<ID3D11DeviceContext> context;
	iptr<IDXGIFactory1> dxgi_factory;
	iptr<IDXGISwapChain> swap_chain;
	iptr<ID3D11RenderTargetView> window_rtv;
	iptr<ID3DX11Effect> effect;

	iptr<ID3D11Texture2D> zbuffer;
	iptr<ID3D11ShaderResourceView> zbuffer_srv;
	iptr<ID3D11DepthStencilView> zbuffer_dsv;

	iptr<ID3D11Texture2D> gbuffer0;
	iptr<ID3D11ShaderResourceView> gbuffer0_srv;
	iptr<ID3D11RenderTargetView> gbuffer0_rtv;
	iptr<ID3D11Texture2D> gbuffer1;
	iptr<ID3D11ShaderResourceView> gbuffer1_srv;
	iptr<ID3D11RenderTargetView> gbuffer1_rtv;

	iptr<ID3D11Texture2D> accum;
	iptr<ID3D11ShaderResourceView> accum_srv;
	iptr<ID3D11RenderTargetView> accum_rtv;

	iptr<ID3D11InputLayout> input_layout_quad;
	iptr<ID3D11InputLayout> input_layout_objects;

	struct
	{
		ID3DX11EffectPass* prepass;
		ID3DX11EffectPass* render;
		ID3DX11EffectPass* directional_light;
		ID3DX11EffectPass* ambient_light;
		ID3DX11EffectPass* sky;
		ID3DX11EffectPass* hdr;
	} pass;

	struct
	{
		ID3DX11EffectMatrixVariable* mWorldViewProj;
		ID3DX11EffectMatrixVariable* mWorldView;
		ID3DX11EffectMatrixVariable* mViewProj;
		ID3DX11EffectMatrixVariable* mView_I;

		ID3DX11EffectVectorVariable* light_pos;
		ID3DX11EffectVectorVariable* light_colour;

		ID3DX11EffectScalarVariable* field_of_view;
		ID3DX11EffectScalarVariable* aspect_ratio;
		ID3DX11EffectScalarVariable* aperture;

		ID3DX11EffectShaderResourceVariable* accum;
		ID3DX11EffectShaderResourceVariable* gbuffer0;
		ID3DX11EffectShaderResourceVariable* gbuffer1;
		ID3DX11EffectShaderResourceVariable* zbuffer;
	} var;

	Geometry quad;
	Settings settings;
	Window window;

public:
	struct
	{
		float yaw;
		float pitch;
	} camera;

	Matrix4f proj;
	Matrix4f start_view;
	float aperture;
	Vector3f ambient;
	
	Renderer(Settings);
	void render(
		std::vector<Object, Eigen::aligned_allocator<Object>>& objects,
		std::vector<Light, Eigen::aligned_allocator<Light>>& lights );

	Geometry read_geom(const std::string& filename);

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif