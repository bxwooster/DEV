#include "DeviceState.hpp"
#include "ShaderCache.hpp"
#include "OK.h"
#include <string>

namespace Devora {
namespace Tools {

void CompileShader( char* file, char* profile, ID3D10Blob** code );

}

namespace LoadShader
{
	void Vertex(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11VertexShader>& shader)
	{
		std::string key(name);
		auto it = cache.vertex_shaders.find(key);
		if (it != cache.vertex_shaders.end())
		{
			shader = it->second;
			return;
		}

		IPtr<ID3D10Blob> code;
		Tools::CompileShader(name, "vs_5_0", ~code);
		HOK( device.device->CreateVertexShader(code->GetBufferPointer(),
			code->GetBufferSize(), NULL, ~shader));
		cache.vertex_shaders[key] = shader;
	}

	void Geometry(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11GeometryShader>& shader)
	{
		std::string key(name);
		auto it = cache.geometry_shaders.find(key);
		if (it != cache.geometry_shaders.end())
		{
			shader = it->second;
			return;
		}

		IPtr<ID3D10Blob> code;
		Tools::CompileShader(name, "gs_5_0", ~code);
		HOK( device.device->CreateGeometryShader(code->GetBufferPointer(),
			code->GetBufferSize(), NULL, ~shader));
		cache.geometry_shaders[key] = shader;
	}

	void Pixel(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11PixelShader>& shader)
	{
		std::string key(name);
		auto it = cache.pixel_shaders.find(key);
		if (it != cache.pixel_shaders.end())
		{
			shader = it->second;
			return;
		}

		IPtr<ID3D10Blob> code;
		Tools::CompileShader(name, "ps_5_0", ~code);
		HOK( device.device->CreatePixelShader(code->GetBufferPointer(),
			code->GetBufferSize(), NULL, ~shader));
		cache.pixel_shaders[key] = shader;
	}
}

} // namespace Devora