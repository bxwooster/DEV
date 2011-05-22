#include "LoadShader.hpp"
#include "OK.hpp"
#include <string>

namespace DEV {
namespace Tools {
void CompileShader( char* file, char* entry, char* profile, ID3D10Blob** code );
}

namespace LoadShader
{
	void Compute(ShaderCache& cache, DeviceState& device, IPtr<ID3D11ComputeShader>& shader, char* file, char* entry)
	{
		std::string key(file);
		key.append("@");
		key.append(entry);
		auto it = cache.compute_shaders.find(key);
		if (it != cache.compute_shaders.end())
		{
			shader = it->second;
			return;
		}

		IPtr<ID3D10Blob> code;
		Tools::CompileShader(file, entry, "cs_5_0", ~code);
		HOK( device.device->CreateComputeShader(code->GetBufferPointer(),
			code->GetBufferSize(), NULL, ~shader));
		cache.compute_shaders[key] = shader;
	}

	void Vertex(ShaderCache& cache, DeviceState& device, IPtr<ID3D11VertexShader>& shader, char* file, char* entry)
	{
		std::string key(file);
		key.append("@");
		key.append(entry);
		auto it = cache.vertex_shaders.find(key);
		if (it != cache.vertex_shaders.end())
		{
			shader = it->second;
			return;
		}

		IPtr<ID3D10Blob> code;
		Tools::CompileShader(file, entry, "vs_5_0", ~code);
		HOK( device.device->CreateVertexShader(code->GetBufferPointer(),
			code->GetBufferSize(), NULL, ~shader));
		cache.vertex_shaders[key] = shader;
	}

	void Geometry(ShaderCache& cache, DeviceState& device, IPtr<ID3D11GeometryShader>& shader, char* file, char* entry)
	{
		std::string key(file);
		key.append("@");
		key.append(entry);
		auto it = cache.geometry_shaders.find(key);
		if (it != cache.geometry_shaders.end())
		{
			shader = it->second;
			return;
		}

		IPtr<ID3D10Blob> code;
		Tools::CompileShader(file, entry, "gs_5_0", ~code);
		HOK( device.device->CreateGeometryShader(code->GetBufferPointer(),
			code->GetBufferSize(), NULL, ~shader));
		cache.geometry_shaders[key] = shader;
	}

	void Pixel(ShaderCache& cache, DeviceState& device, IPtr<ID3D11PixelShader>& shader, char* file, char* entry)
	{
		std::string key(file);
		key.append("@");
		key.append(entry);
		auto it = cache.pixel_shaders.find(key);
		if (it != cache.pixel_shaders.end())
		{
			shader = it->second;
			return;
		}

		IPtr<ID3D10Blob> code;
		Tools::CompileShader(file, entry, "ps_5_0", ~code);
		HOK( device.device->CreatePixelShader(code->GetBufferPointer(),
			code->GetBufferSize(), NULL, ~shader));
		cache.pixel_shaders[key] = shader;
	}
}

} // namespace DEV