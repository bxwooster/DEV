#ifndef __Devora_LightRenderInfo__
#define __Devora_LightRenderInfo__

#include "Matrix.h"

namespace Devora {

struct LightRenderInfo
{
	Matrix4f proj;
	Matrix4f cubematrices[6];

	IPtr<ID3D11InputLayout> layout;
	IPtr<ID3D11RasterizerState> rs_shadow;
	IPtr<ID3D11RasterizerState> rs_both_sides;
	IPtr<ID3D11BlendState> bs_additive;
	IPtr<ID3D11SamplerState> sm_point;

	IPtr<ID3D11VertexShader> vs_render_z;
	IPtr<ID3D11VertexShader> vs_render_cube_z;
	IPtr<ID3D11GeometryShader> gs_render_cube_z;
	IPtr<ID3D11VertexShader> vs_noop;
	IPtr<ID3D11GeometryShader> gs_fullscreen;
	IPtr<ID3D11GeometryShader> gs_dir_light;
	IPtr<ID3D11PixelShader> ps_dir_light;
	IPtr<ID3D11PixelShader> ps_point_light;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif