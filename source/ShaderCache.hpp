#ifndef __Devora_ShaderCache__
#define __Devora_ShaderCache__

#include "IPtr.h"
#include <D3D11.h>
#include <unordered_map>
#include <string>

namespace Devora {

struct ShaderCache
{
	IPtr<ID3D11ClassLinkage> linkage;
	std::unordered_map<std::string, IPtr<ID3D11VertexShader>> vertex_shaders;
	std::unordered_map<std::string, IPtr<ID3D11GeometryShader>> geometry_shaders;
	std::unordered_map<std::string, IPtr<ID3D11PixelShader>> pixel_shaders;
};

} // namespace Devora

#endif