#ifndef __Freddy_h__
#define __Freddy_h__

#include <list>
#include <memory>

#include <d3d11.h>
#include <d3dx11effect.h>

#include "Matrix.h"
#include "iPtr.h"

#include "Input.h"

using namespace common;

typedef unsigned int uint;

class Freddy
{

public:
	struct Settings
	{
		int width;
		int height;
		float mouse_sens;
	};

	class Quit {};

private:
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

	struct
	{
		float yaw;
		float pitch;
	} camera;

	Settings settings;

	HWND window_handle;
	iptr<ID3D11Device> device;
	iptr<ID3D11DeviceContext> context;
	iptr<IDXGIFactory1> dxgi_factory;
	iptr<IDXGISwapChain> swap_chain;
	iptr<ID3D11RenderTargetView> window_rtv;
	iptr<ID3DX11Effect> effect;

	iptr<ID3D11Texture2D> depth;
	iptr<ID3D11ShaderResourceView> depth_srv;
	iptr<ID3D11DepthStencilView> depth_dsv;

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
		ID3DX11EffectPass* light_point;
		ID3DX11EffectPass* light_ambient;
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

	Matrix4f proj;
	Matrix4f start_view;
	float aperture;
	Vector3f ambient;

	Geometry quad;

	std::vector<Object, Eigen::aligned_allocator<Object>> objects;

	void on_key(int key, bool up);
	void on_mouse(long dx, long dy);

	Geometry read_geom(const std::string& filename);
public:
	Freddy(Settings settings_);
	void step(Input& input);

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif