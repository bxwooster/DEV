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

void RenderLights(GraphicsState& state, VisualRenderInfo& vinfo, LightRenderInfo& info,
	Transforms& transforms, Lights& lights, Visuals& casters, Camera& camera,
	ZBuffer& zbuffer, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& gbuffer0, Buffer& gbuffer1, Buffer& lbuffer,
	CBuffer& cb_frame, CBuffer& cb_object_z, CBuffer& cb_object_cube_z, CBuffer& cb_light)
{
	state.context->ClearState();

	const float blendf[4] = {1.0f, 1.0f, 1.0f, 0.0f};
	const float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	state.context->ClearRenderTargetView(lbuffer.rtv, black);

	state.context->PSSetSamplers(0, 1, &info.sm_point);

	HOK( state.var.gbuffer0->SetResource( gbuffer0.srv ) );
	HOK( state.var.gbuffer1->SetResource( gbuffer1.srv ) );
	HOK( state.var.zbuffer->SetResource( zbuffer.srv ) );

	Matrix4f view_axis;
	view_axis << 0, 1, 0, 0,
				 0, 0, 1, 0,
				 1, 0, 0, 0,
				 0, 0, 0, 1;

	for (int k = 0; k < lights.dir.size(); k++)
	{
		Light& light = lights.dir[k];

		Matrix4f lightview = view_axis * transforms[light.index].inverse();
		Matrix4f lightview_lightproj = info.proj * lightview;

		state.context->ClearDepthStencilView(shadowmap.dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
		state.context->OMSetRenderTargets(0, NULL, shadowmap.dsv);
		state.context->IASetInputLayout( vinfo.layout );
		state.context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		state.context->RSSetViewports( 1, &shadowmap.viewport );
		state.context->VSSetConstantBuffers(2, 1, &cb_object_z);

		state.context->VSSetShader( info.vs_render_z, NULL, 0 );
		state.context->GSSetShader( NULL, NULL, 0 );
		state.context->PSSetShader( NULL, NULL, 0 );
		state.context->RSSetState( info.rs_shadow );
		state.context->OMSetBlendState( NULL, blendf, 0xffffffff );
		state.context->OMSetDepthStencilState( NULL, 0u );

		for (uint i = 0; i < casters.size(); i++)
		{
			Visual& caster = casters[i];
			Geometry& geom = vinfo.geoms[caster.type];

			CBufferLayouts::object_z data;
			data.world_lightview_lightproj = lightview_lightproj * transforms[caster.index];

			state.context->UpdateSubresource(cb_object_z, 0, NULL, (void*)&data, sizeof(data), 0);
			state.context->IASetVertexBuffers(0, 1, &geom.buffer, &geom.stride, &geom.offset);
			state.context->Draw( geom.count, 0 );
		}

		CBufferLayouts::light data;
		data.light_matrix = lightview * camera.view.inverse();
		data.reproject = info.proj * data.light_matrix;
		data.light_pos = (camera.view * transforms[light.index].col(3)).head<3>();
		data.light_colour = light.colour;

		ID3D11Buffer* buffers[5] = { cb_frame, NULL, NULL, NULL, cb_light };
		ID3D11ShaderResourceView* views[4] = { gbuffer0.srv, gbuffer1.srv, zbuffer.srv, shadowmap.srv };

		state.context->UpdateSubresource(cb_light, 0, NULL, (void*)&data, sizeof(data), 0);
		state.context->OMSetRenderTargets(1, &lbuffer.rtv, NULL);
		state.context->IASetInputLayout( NULL );
		state.context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		state.context->RSSetViewports( 1, &lbuffer.viewport );
		state.context->PSSetConstantBuffers(0, 5, buffers);
		state.context->PSSetShaderResources(0, 4, views);

		state.context->VSSetShader( info.vs_noop, NULL, 0 );
		state.context->GSSetShader( info.gs_fullscreen, NULL, 0 );
		state.context->PSSetShader( info.ps_dir_light, NULL, 0 );
		state.context->OMSetBlendState( info.bs_additive, blendf, 0xffffffff );
		state.context->OMSetDepthStencilState( info.ds_nowrite, 0 );
		state.context->RSSetState( vinfo.rs_default );

		state.context->Draw( 1, 0 );
	}

	for (int k = 0; k < lights.point.size(); k++)
	{
		Light& light = lights.point[k];

		state.context->IASetInputLayout( vinfo.layout );
		state.context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		Matrix4f lightview = view_axis * transforms[light.index].inverse();
		Matrix4f lightview_lightproj = info.proj * lightview;

		Matrix4f light_matrix = lightview * camera.view.inverse();
		HOK( state.var.light_matrix->SetMatrix( light_matrix.data() ));
		Matrix4f reproject = info.proj * light_matrix;
		HOK( state.var.reproject->SetMatrix( reproject.data() ));

		state.context->ClearDepthStencilView
			(shadowcube.dsv, D3D11_CLEAR_DEPTH, 1.0, 0);

		auto v = shadowcube.viewport;
		D3D11_VIEWPORT viewports[6] = {v, v, v, v, v, v};
		state.context->RSSetViewports( 6, viewports );

		for (uint i = 0; i < casters.size(); i++)
		{
			{
				Matrix4f p = info.proj;
				Matrix4f w = lightview * transforms[casters[i].index];
				Matrix4f cubeproj[6];
				for (int f = 0; f < 6; f++) cubeproj[f] = p * info.cubematrices[f] * w;
				HOK( state.var.cubeproj->SetMatrixArray((float*) cubeproj, 0, 6));
			}
				
			HOK( state.var.shadowcube->SetResource( NULL ) );
			HOK( state.pass_render_cube_z->Apply( 0, state.context ) );
			state.context->OMSetRenderTargets(0, NULL, shadowcube.dsv);
		
			Geometry& geom = vinfo.geoms[casters[i].type];

			state.context->IASetVertexBuffers
				(0, 1, &geom.buffer, &geom.stride, &geom.offset);
			state.context->Draw( geom.count, 0 );
		}

		state.context->IASetInputLayout( NULL );
		state.context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		state.context->RSSetViewports( 1, &lbuffer.viewport );
		state.context->OMSetRenderTargets(1, &lbuffer.rtv, NULL);

		Vector4f position(camera.view * transforms[light.index].col(3));
		HOK( state.var.light_pos->SetRawValue
			( (void*)position.data(), 0, sizeof(Vector3f) ) );

		Vector3f& colour = light.colour;
		HOK( state.var.light_colour->SetRawValue
			( (void*)colour.data(), 0, sizeof(Vector3f) ) );
		
		HOK( state.var.shadowcube->SetResource( shadowcube.srv ) );
		HOK( state.pass_point_light->Apply( 0, state.context ) );

		state.context->Draw( 1, 0 );
	}
}

} // namespace Devora