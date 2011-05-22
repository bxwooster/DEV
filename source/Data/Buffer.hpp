#pragma once
#include "IPtr.hpp"
#include <d3d11.h>

namespace DEV {

struct Buffer
{
	IPtr<ID3D11Texture2D> texture;
	IPtr<ID3D11ShaderResourceView> srv;
	IPtr<ID3D11RenderTargetView> rtv;
	D3D11_VIEWPORT viewport;
};

} // namespace DEV
