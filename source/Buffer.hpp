#ifndef __Devora_Buffer__
#define __Devora_Buffer__

#include "IPtr.h"

#include <d3d11.h>

namespace Devora {

struct Buffer
{
	IPtr<ID3D11Texture2D> texture;
	IPtr<ID3D11ShaderResourceView> srv;
	IPtr<ID3D11RenderTargetView> rtv;
	D3D11_VIEWPORT viewport;
};

} // namespace Devora

#endif