#include "OK.h"

#include "Camera.hpp"
#include "DeviceState.hpp"
#include "UBuffer.hpp"
#include "shaders/struct/OITFragment"

namespace DEV {

void InitOIT(DeviceState& device, Camera& camera, UBuffer& start_buffer, UBuffer& fragment_buffer)
{
	int screen_size = camera.screen.w * camera.screen.h;
	int n_fragments = 2 * screen_size; //!
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = n_fragments * sizeof(OITFragment);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(OITFragment);
	
		HOK( device.device->CreateBuffer
			(&desc, NULL, ~fragment_buffer.buffer));

		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.ByteWidth = screen_size * sizeof(int);

		HOK( device.device->CreateBuffer
			(&desc, NULL, ~start_buffer.buffer));
	}
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = n_fragments;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
		desc.Format = DXGI_FORMAT_UNKNOWN;

		HOK( device.device->CreateUnorderedAccessView
			( fragment_buffer.buffer, &desc, ~fragment_buffer.uav ));

		desc.Buffer.NumElements = screen_size;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;

		HOK( device.device->CreateUnorderedAccessView
			( start_buffer.buffer, &desc, ~start_buffer.uav ));
	}
}

} // namespace DEV