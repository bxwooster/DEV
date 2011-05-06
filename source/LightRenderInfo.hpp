#ifndef __Devora_LightRenderInfo__
#define __Devora_LightRenderInfo__

#include "Matrix.h"

namespace Devora {

struct LightRenderInfo
{
	Matrix4f proj;
	Matrix4f cubematrices[6];

	IPtr<ID3D11InputLayout> layout;

	IPtr<ID3D11VertexShader> vshader_z;

	IPtr<ID3D11VertexShader> vshader_cube_z;
	IPtr<ID3D11GeometryShader> gshader_cube_z;

	IPtr<ID3D11VertexShader> vshader_fs;
	IPtr<ID3D11GeometryShader> gshader_fs;
	
	IPtr<ID3D11PixelShader> pshader_directional;
	IPtr<ID3D11PixelShader> pshader_point;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif