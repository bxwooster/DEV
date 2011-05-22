#pragma once
#include "Data/Geometries.hpp"
#include "Data/DeviceState.hpp"
#include "Matrix.hpp"

namespace DEV {

namespace Tools {

void InitCBuffers(DeviceState& device,	ID3D11Buffer** buffers[], size_t* sizes, int number);

Geometry ReadGeometry(ID3D11Device* device, const std::string& filename);
void SetProjectionMatrix(Matrix4f& proj, float y_fov, float aspect_ratio, float z_near);
void CompileShader( char* file, char* profile, char* entry, ID3D10Blob** code );

D3D11_BLEND_DESC DefaultBlendDesc();
D3D11_SAMPLER_DESC DefaultSamplerDesc();
D3D11_DEPTH_STENCIL_DESC DefaultDepthStencilDesc();
D3D11_RASTERIZER_DESC DefaultRasterizerDesc();

}

} // namespace DEV
