#pragma once
#include "IPtr.hpp"
#include <D3D11.h>

namespace DEV {

struct ZBuffer
{
	IPtr<ID3D11Texture2D> texture;
	IPtr<ID3D11ShaderResourceView> srv;
	IPtr<ID3D11DepthStencilView> dsv;
	IPtr<ID3D11DepthStencilView> dsv_readonly;
	D3D11_VIEWPORT viewport;
};

} // namespace DEV
