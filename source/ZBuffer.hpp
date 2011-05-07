#ifndef __Devora_ZBuffer__
#define __Devora_ZBuffer__

#include "IPtr.h"
#include <D3D11.h>

namespace Devora {

struct ZBuffer
{
	IPtr<ID3D11Texture2D> texture;
	IPtr<ID3D11ShaderResourceView> srv;
	IPtr<ID3D11DepthStencilView> dsv;
	D3D11_VIEWPORT viewport;
};

} // namespace Devora

#endif