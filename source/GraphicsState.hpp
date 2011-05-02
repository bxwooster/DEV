#ifndef __Devora_GraphicsState__
#define __Devora_GraphicsState__

#include "Matrix.h"
#include "IPtr.h"

#include <d3d11.h>
#include <d3dx11effect.h>

namespace Devora {

struct GraphicsState
{
	IPtr<ID3D11Device> device;
	IPtr<ID3D11DeviceContext> context;
	IPtr<IDXGISwapChain> swap_chain;
	IPtr<ID3DX11Effect> effect;

	ID3DX11EffectPass* pass_render;
	ID3DX11EffectPass* pass_render_z;
	ID3DX11EffectPass* pass_render_cube_z;
	ID3DX11EffectPass* pass_directional_light;
	ID3DX11EffectPass* pass_point_light;
	ID3DX11EffectPass* pass_final;

	struct
	{
		ID3DX11EffectMatrixVariable* world_lightview_lightproj;
		ID3DX11EffectMatrixVariable* world_lightview;
		ID3DX11EffectMatrixVariable* world_view_proj;
		ID3DX11EffectMatrixVariable* world_view;
		ID3DX11EffectMatrixVariable* view;
		ID3DX11EffectMatrixVariable* view_i;
		ID3DX11EffectMatrixVariable* reproject;
		ID3DX11EffectMatrixVariable* cubeproj;

		ID3DX11EffectMatrixVariable* light_matrix;
		ID3DX11EffectVectorVariable* light_pos;
		ID3DX11EffectVectorVariable* light_colour;

		ID3DX11EffectScalarVariable* field_of_view;
		ID3DX11EffectScalarVariable* aspect_ratio;
		ID3DX11EffectScalarVariable* aperture;
		ID3DX11EffectScalarVariable* z_near;

		ID3DX11EffectShaderResourceVariable* lbuffer;
		ID3DX11EffectShaderResourceVariable* gbuffer0;
		ID3DX11EffectShaderResourceVariable* gbuffer1;
		ID3DX11EffectShaderResourceVariable* zbuffer;
		ID3DX11EffectShaderResourceVariable* shadowmap;
		ID3DX11EffectShaderResourceVariable* shadowcube;
	} var;

	Matrix4f cubematrices[6];
	float field_of_view;
	float aspect_ratio;
	float z_near;
	float aperture;
	HWND window;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif