#pragma once

#include "IPtr.h"
#include <D3D11.h>
#include <unordered_map>
#include <string>

namespace DEV {

struct ShaderCache
{
	IPtr<ID3D11ClassLinkage> linkage;
	std::unordered_map<std::string, IPtr<ID3D11VertexShader>> vertex_shaders;
	std::unordered_map<std::string, IPtr<ID3D11GeometryShader>> geometry_shaders;
	std::unordered_map<std::string, IPtr<ID3D11PixelShader>> pixel_shaders;
	std::unordered_map<std::string, IPtr<ID3D11ComputeShader>> compute_shaders;
};

} // namespace DEV
