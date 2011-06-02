#include "RenderLights.hpp"
#include "CBufferLayouts.hpp"

namespace DEV {

typedef unsigned int uint;

void RenderLights::run()
{
	ID3D11UnorderedAccessView* uavs[] = { oit_start.uav, oit_consolidated.uav, oit_scattered.uav };
	unsigned int counts[] = { -1, -1, -1 };

	const float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	state->ClearRenderTargetView(lbuffer.rtv, black);

	state->ClearState();

	state->OMSetRenderTargetsAndUnorderedAccessViews(0, NULL, NULL, 0, 3, uavs, counts);
	state->RSSetViewports( 1, &zbuffer.viewport );
	state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

	state->VSSetShader(info.vs_noop, NULL, 0);
	state->GSSetShader(info.gs_fullscreen, NULL, 0);
	state->PSSetShader( info.ps_oit_consolidate, NULL, 0 );

	state->PSSetConstantBuffers( 0, 1, &cb_frame );
	state->PSSetShaderResources(0, 1, &zbuffer.srv);
	state->PSSetSamplers(0, 1, &info.sm_point);

	state->Draw(1, 0);

	state->ClearState();

	//state->OMSetRenderTargets(1, &lbuffer.rtv, zbuffer.dsv);
	//state->RSSetViewports( 1, &lbuffer.viewport );
	//state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

	//state->RSSetState(info.rs_default);
	//state->OMSetBlendState( NULL, blendf, 0xffffffff );
	//state->OMSetDepthStencilState( info.ds_less_equal, 0 );

	//state->VSSetShader(info.vs_noop, NULL, 0);
	//state->GSSetShader(info.gs_fullscreen, NULL, 0);
	//state->PSSetShader(info.ps_skylight, NULL, 0);
	//
	//state->PSSetConstantBuffers(0, 1, &cb_frame);
	//state->PSSetSamplers(0, 1, &info.sm_point);

	//state->Draw( 1, 0 );

	Parameters P1 = { lights.dir, shadowmap,
		info.vs_render_z, NULL, cb_object_z,
		info.gs_dir_light, info.ps_dir_light };
	shared_code(P1);

	Parameters P2 = { lights.point, shadowcube,
		info.vs_render_cube_z, info.gs_render_cube_z, cb_object_cube_z,
		info.gs_point_light, info.ps_point_light };
	shared_code(P2);
}

void RenderLights::shared_code(Parameters& P)
{
	ID3D11UnorderedAccessView* uavs[] = { oit_start.uav, oit_consolidated.uav };
	unsigned int counts[] = { -1, -1 };

	const float blendfactors[4] = {1.0f, 1.0f, 1.0f, 1.0f};

	for (int k = 0; k < P.lights.size(); k++)
	{
		Light const& light = P.lights[k];

		Matrix4f lightview = transforms[light.transform].inverse();

		state->ClearDepthStencilView(P.shadowsurface.dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
		state->ClearState();

		state->OMSetRenderTargets(0, NULL, P.shadowsurface.dsv);
		state->RSSetViewports( 1, &P.shadowsurface.viewport );
		state->IASetInputLayout( info.layout_z );
		state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		state->RSSetState( info.rs_shadow );
		state->OMSetBlendState( NULL, blendfactors, 0xffffffff );
		state->OMSetDepthStencilState( NULL, 0 );

		state->VSSetShader( P.vs_render, NULL, 0 );
		state->GSSetShader( P.gs_render, NULL, 0 );
		state->PSSetShader( NULL, NULL, 0 );

		state->VSSetConstantBuffers(0, 1, &P.cbuffer);
		state->GSSetConstantBuffers(0, 1, &P.cbuffer);

		for (uint i = 0; i < casters.size(); i++)
		{
			Geometry& geom = geometries[casters[i].geometry];

			Matrix4f view = lightview * transforms[casters[i].transform];

			if (P.cbuffer == cb_object_cube_z)
			{
				CBufferLayouts::object_cube_z data;
				for (int f = 0; f < 6; f++) data.cubeproj[f] = info.proj * info.cubematrices[f] * view;
				state->UpdateSubresource(cb_object_cube_z, 0, NULL, (void*)&data, sizeof(data), 0);
				state->GSSetConstantBuffers(0, 1, &cb_object_cube_z);
			} 
			else if (P.cbuffer == cb_object_z)
			{
				CBufferLayouts::object_z data;
				data.world_view_proj = info.proj * view;
				state->UpdateSubresource(cb_object_z, 0, NULL, (void*)&data, sizeof(data), 0);
				state->VSSetConstantBuffers(0, 1, &cb_object_z);
			}

			state->IASetVertexBuffers(0, 1, &*geom.buffers, geom.strides, geom.offsets);
			state->IASetIndexBuffer( geom.buffers[2], DXGI_FORMAT_R16_UINT, 0 );
			state->DrawIndexed( geom.count, 0, 0 );
		}

		CBufferLayouts::light data;
		data.viewI_light_view = lightview * camera.view.inverse();
		data.viewI_light_view_proj = info.proj * data.viewI_light_view;
		data.light_pos = (camera.view * transforms[light.transform].col(3)).head<3>();
		data.light_colour = light.colour;
		data.light_world_view_proj = camera.proj * camera.view * transforms[light.transform];
		data.radius = 30; //!

		ID3D11Buffer* buffers[] = { cb_frame, cb_light };
		ID3D11ShaderResourceView* views[] = { gbuffer.srv, zbuffer.srv, P.shadowsurface.srv };

		state->UpdateSubresource(cb_light, 0, NULL, (void*)&data, sizeof(data), 0);

		state->OMSetRenderTargetsAndUnorderedAccessViews(1, &lbuffer.rtv, NULL, 1, 2, uavs, counts);
		state->IASetInputLayout( NULL );
		state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		state->RSSetViewports( 1, &lbuffer.viewport );

		state->RSSetState( info.rs_backface );
		state->OMSetBlendState( info.bs_additive, blendfactors, 0xffffffff );
		state->OMSetDepthStencilState( NULL, 0 );

		state->VSSetShader( info.vs_noop, NULL, 0 );
		state->GSSetShader( P.gs_light, NULL, 0 );
		state->PSSetShader( P.ps_light, NULL, 0 );

		state->GSSetConstantBuffers(0, 1, &cb_light);
		state->PSSetConstantBuffers(0, 2, buffers);
		state->PSSetShaderResources(0, 3, views);
		state->PSSetSamplers(0, 1, &info.sm_point);

		state->Draw( 1, 0 );
	}
}

} // namespace DEV