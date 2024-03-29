#pragma once
#include "Matrix.hpp"
#include "IPtr.hpp"
#include <D3D11.h>

namespace DEV {

struct LightRenderInfo
{
	Matrix4f proj;
	Matrix4f cubematrices[6];

	IPtr<ID3D11InputLayout> layout;
	IPtr<ID3D11InputLayout> layout_z;
	IPtr<ID3D11RasterizerState> rs_default;
	IPtr<ID3D11RasterizerState> rs_shadow;
	IPtr<ID3D11RasterizerState> rs_backface;
	IPtr<ID3D11BlendState> bs_additive;
	IPtr<ID3D11DepthStencilState> ds_less_equal;
	IPtr<ID3D11SamplerState> sm_point;

	IPtr<ID3D11VertexShader> vs_render_z;
	IPtr<ID3D11VertexShader> vs_render_cube_z;
	IPtr<ID3D11GeometryShader> gs_render_cube_z;
	IPtr<ID3D11VertexShader> vs_noop;
	IPtr<ID3D11GeometryShader> gs_fullscreen;
	IPtr<ID3D11GeometryShader> gs_dir_light;
	IPtr<ID3D11GeometryShader> gs_point_light;
	IPtr<ID3D11PixelShader> ps_dir_light;
	IPtr<ID3D11PixelShader> ps_point_light;
	IPtr<ID3D11PixelShader> ps_skylight;

	IPtr<ID3D11PixelShader> ps_oit_consolidate;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace DEV
