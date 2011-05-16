#include "DeviceState.hpp"
#include "ShaderCache.hpp"

namespace DEV {
namespace LoadShader {

void Compute(ShaderCache& cache, DeviceState& device, IPtr<ID3D11ComputeShader>& shader, char* name, char* entry = "main");
void Vertex(ShaderCache& cache, DeviceState& device, IPtr<ID3D11VertexShader>& shader, char* name, char* entry = "main");
void Geometry(ShaderCache& cache, DeviceState& device, IPtr<ID3D11GeometryShader>& shader, char* name, char* entry = "main");
void Pixel(ShaderCache& cache, DeviceState& device, IPtr<ID3D11PixelShader>& shader, char* name, char* entry = "main");

} // namespace
} // namespace DEV