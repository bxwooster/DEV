#include "OK.h"

#include "Camera.hpp"
#include "DeviceState.hpp"
#include "UBuffer.hpp"

typedef unsigned int uint;
#include "shaders/struct/OITFragment"

namespace DEV {

void InitOIT(DeviceState& device, Camera& camera,
	UBuffer& oit_start, UBuffer& oit_scattered,
	UBuffer& oit_consolidated)
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
			(&desc, NULL, ~oit_scattered.buffer));

		HOK( device.device->CreateBuffer
			(&desc, NULL, ~oit_consolidated.buffer));

		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.ByteWidth = (screen_size + 2) * sizeof(int); // 2 counters

		HOK( device.device->CreateBuffer
			(&desc, NULL, ~oit_start.buffer));
	}
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = n_fragments;
		desc.Buffer.Flags = 0;
		desc.Format = DXGI_FORMAT_UNKNOWN;

		HOK( device.device->CreateUnorderedAccessView
			( oit_consolidated.buffer, &desc, ~oit_consolidated.uav ));

		desc.Buffer.Flags = desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

		HOK( device.device->CreateUnorderedAccessView
			( oit_scattered.buffer, &desc, ~oit_scattered.uav ));

		desc.Buffer.NumElements = screen_size + 2; //!
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;

		HOK( device.device->CreateUnorderedAccessView
			( oit_start.buffer, &desc, ~oit_start.uav ));
	}
}

} // namespace DEV