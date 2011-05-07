#include "Transforms.hpp"
#include "GraphicsState.hpp"
#include "LightRenderInfo.hpp"
#include "VisualRenderInfo.hpp"
#include "Lights.hpp"
#include "Camera.hpp"
#include "Visuals.hpp"
#include "ZBuffer.hpp"
#include "Buffer.hpp"
#include "CBuffer.hpp"
#include "CBufferLayouts.hpp"

#include <d3dx11.h>

namespace Devora {

typedef unsigned int uint;

void RenderLights(GraphicsState& state, LightRenderInfo& info,
	Transforms& transforms, Lights& lights, Visuals& casters, Camera& camera,
	ZBuffer& zbuffer, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& gbuffer0, Buffer& gbuffer1, Buffer& lbuffer,
	CBuffer& cb_frame, CBuffer& cb_object_z, CBuffer& cb_object_cube_z, CBuffer& cb_light)
{
	const float blendf[4] = {1.0f, 1.0f, 1.0f, 0.0f};
	const float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	state->ClearRenderTargetView(lbuffer.rtv, black);

	for (int k = 0; k < lights.dir.size(); k++)
	{
		Light& light = lights.dir[k];

		Matrix4f lightview = transforms[light.index].inverse();
		Matrix4f lightview_lightproj = info.proj * lightview;

		state->ClearDepthStencilView(shadowmap.dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
		state->ClearState();
		state->OMSetRenderTargets(0, NULL, shadowmap.dsv);
		state->IASetInputLayout( info.layout );
		state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		state->RSSetViewports( 1, &shadowmap.viewport );
		state->VSSetConstantBuffers(0, 1, &cb_object_z);

		state->VSSetShader( info.vs_render_z, NULL, 0 );
		state->GSSetShader( NULL, NULL, 0 );
		state->PSSetShader( NULL, NULL, 0 );
		state->RSSetState( info.rs_shadow );
		state->OMSetBlendState( NULL, blendf, 0xffffffff );
		state->OMSetDepthStencilState( NULL, 0u );

		for (uint i = 0; i < casters.size(); i++)
		{
			Geometry& geom = info.geoms[casters[i].type];

			CBufferLayouts::object_z data;
			data.world_view_proj = lightview_lightproj * transforms[casters[i].index];

			state->UpdateSubresource(cb_object_z, 0, NULL, (void*)&data, sizeof(data), 0);
			state->IASetVertexBuffers(0, 1, &geom.buffer, &geom.stride, &geom.offset);
			state->Draw( geom.count, 0 );
		}

		CBufferLayouts::light data;
		data.viewI_light_view = lightview * camera.view.inverse();
		data.viewI_light_view_proj = info.proj * data.viewI_light_view;
		data.light_pos = (camera.view * transforms[light.index].col(3)).head<3>();
		data.light_colour = light.colour;
		data.light_world_view_proj = camera.proj * camera.view * transforms[light.index];
		data.radius = 30; //!

		ID3D11Buffer* buffers[5] = { cb_frame, cb_light };
		ID3D11ShaderResourceView* views[4] = { gbuffer0.srv, gbuffer1.srv, zbuffer.srv, shadowmap.srv };

		state->UpdateSubresource(cb_light, 0, NULL, (void*)&data, sizeof(data), 0);
		state->OMSetRenderTargets(1, &lbuffer.rtv, NULL);
		state->IASetInputLayout( NULL );
		state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		state->RSSetViewports( 1, &lbuffer.viewport );

		state->RSSetState( info.rs_default );
		state->OMSetBlendState( info.bs_additive, blendf, 0xffffffff );
		state->OMSetDepthStencilState( NULL, 0 );

		state->VSSetShader( info.vs_noop, NULL, 0 );
		state->GSSetShader( info.gs_dir_light, NULL, 0 );
		state->PSSetShader( info.ps_dir_light, NULL, 0 );

		state->GSSetConstantBuffers(0, 1, &cb_light);
		state->PSSetConstantBuffers(0, 2, buffers);
		state->PSSetShaderResources(0, 4, views);
		state->PSSetSamplers(0, 1, &info.sm_point);
		

		state->Draw( 1, 0 );
	}


	for (int k = 0; k < lights.point.size(); k++)
	{
		Light& light = lights.point[k];

		Matrix4f lightview = transforms[light.index].inverse();
		Matrix4f lightview_lightproj = info.proj * lightview;

		D3D11_VIEWPORT& v = shadowcube.viewport;
		D3D11_VIEWPORT viewports[6] = {v, v, v, v, v, v};

		state->ClearDepthStencilView(shadowcube.dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
		state->ClearState();
		state->OMSetRenderTargets(0, NULL, shadowcube.dsv);
		state->IASetInputLayout( info.layout );
		state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		state->RSSetViewports( 6, viewports );
		state->GSSetConstantBuffers(0, 1, &cb_object_cube_z);

		state->VSSetShader( info.vs_render_cube_z, NULL, 0 );
		state->GSSetShader( info.gs_render_cube_z, NULL, 0 );
		state->PSSetShader( NULL, NULL, 0 );
		state->RSSetState( info.rs_shadow );
		state->OMSetBlendState( NULL, blendf, 0xffffffff );
		state->OMSetDepthStencilState( NULL, 0u );

		for (uint i = 0; i < casters.size(); i++)
		{
			Geometry& geom = info.geoms[casters[i].type];

			Matrix4f& p = info.proj;
			Matrix4f w = lightview * transforms[casters[i].index];

			CBufferLayouts::object_cube_z data;
			for (int f = 0; f < 6; f++) data.cubeproj[f] = p * info.cubematrices[f] * w;	

			state->UpdateSubresource(cb_object_cube_z, 0, NULL, (void*)&data, sizeof(data), 0);
			state->IASetVertexBuffers(0, 1, &geom.buffer, &geom.stride, &geom.offset);
			state->Draw( geom.count, 0 );
		}

		CBufferLayouts::light data;
		data.viewI_light_view = lightview * camera.view.inverse();
		data.viewI_light_view_proj = info.proj * data.viewI_light_view;
		data.light_pos = (camera.view * transforms[light.index].col(3)).head<3>();
		data.light_colour = light.colour;
		data.radius = 30; //!

		ID3D11Buffer* buffers[2] = { cb_frame, cb_light };
		ID3D11ShaderResourceView* views[4] = { gbuffer0.srv, gbuffer1.srv, zbuffer.srv, shadowcube.srv };

		state->UpdateSubresource(cb_light, 0, NULL, (void*)&data, sizeof(data), 0);
		state->OMSetRenderTargets(1, &lbuffer.rtv, NULL);
		state->IASetInputLayout( NULL );
		state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		state->RSSetViewports( 1, &lbuffer.viewport );
		state->PSSetConstantBuffers(0, 2, buffers);
		state->PSSetShaderResources(0, 4, views);
		state->PSSetSamplers(0, 1, &info.sm_point);

		state->VSSetShader( info.vs_noop, NULL, 0 );
		state->GSSetShader( info.gs_fullscreen, NULL, 0 );
		state->PSSetShader( info.ps_point_light, NULL, 0 );
		state->OMSetBlendState( info.bs_additive, blendf, 0xffffffff );
		state->OMSetDepthStencilState( NULL, 0 );
		state->RSSetState( info.rs_default );

		state->Draw( 1, 0 );
	}
}

} // namespace Devora