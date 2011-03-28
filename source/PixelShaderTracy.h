#ifndef __PixelShaderTracy_h__
#define __PixelShaderTracy_h__

#include <list>

#include <d3d11.h>
#include <d3dx11effect.h>

#include "iPtr.h"
#include "Matrix.h"

#include "DefaultInput.h"

using namespace common;

class PixelShaderTracy
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
	iptr<ID3D11RenderTargetView> target;
	iptr<ID3DX11Effect> effect;

	ID3DX11EffectPass* pass;
	ID3DX11EffectMatrixVariable* var_view;
	ID3DX11EffectScalarVariable* var_interp;
	ID3DX11EffectScalarVariable* var_fov;
	ID3DX11EffectScalarVariable* var_aspect;
	ID3DX11EffectScalarVariable* var_steps;

	Matrix4f start_view;
	Matrix4f view;
	float interp;
	Vector4f eye;
	int steps;

	void on_key(int key, bool up);
	void on_mouse(long dx, long dy);

public:
	PixelShaderTracy(Settings settings_);
	void step(DefaultInput& input);
};

#endif