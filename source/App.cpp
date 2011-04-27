#include "App.h"
#include "Ok.h"

namespace Devora {

void App::on_key(int key, bool up)
{
	if(!up)
	{
		switch(key)
		{
		case 27: // ESC
			throw Quit();
			break;
		case 32: // Space
			player_jump = true;
			break;
		}
	}
}

void App::on_held_key(int key)
{
	switch(key)
		{
		case 87: // W
			player_movement += Vector2f(1.0, 0.0);
			break;
		case 83: // S
			player_movement -= Vector2f(1.0, 0.0);
			break;
		case 65: // A
			player_movement += Vector2f(0.0, 1.0);
			break;
		case 68: // D
			player_movement -= Vector2f(0.0, 1.0);
			break;
		}
}


void App::on_mouse(long dx, long dy)
{
	renderer.camera.yaw += dx * settings.mouse_sens;
	renderer.camera.pitch += dy * settings.mouse_sens;
	renderer.camera.pitch = max(-90, min(renderer.camera.pitch, 90));
}

App::App(Settings settings_) :
	renderer(object, light, settings_.r)
{
	settings = settings_;

	auto plane = renderer.read_geom("geometry//plane.geom");
	auto sphere = renderer.read_geom("geometry//icosphere.geom");

	object.transforms.push_back( Matrix4f::Identity() );
	object.geometries.push_back( plane );

	Matrix4f t = Matrix4f::Identity();
	for (int i = -2; i <= 2; i++)
	{
		for (int j = -2; j <= 2; j++)
		{
			t.col(3) = Vector4f(i * 3.0f + j % 2, j * 3.0f + i % 2, 1.0f, 1.0f);
			object.transforms.push_back( t );
			object.geometries.push_back( sphere );
		}
	}

	t = Matrix4f::Identity();
	t.col(3) = Vector4f(0.0, 0.0, 5.0, 1.0);
	light.transforms.push_back( t );
	light.colours.push_back( Vector3f(0.5, 0.5, 0.0) );
	light.types.push_back( Renderer::LightType_point );

	t = Matrix4f::Identity();
	t.col(3) = Vector4f(0.0, 7.0, 1.0, 1.0);
	light.transforms.push_back( t );
	light.colours.push_back( Vector3f(0.0, 0.0, 1.0) );
	light.types.push_back( Renderer::LightType_directional );

	t = Matrix4f::Identity();
	t.col(3) = Vector4f(0.0, -7.0, 1.0, 1.0);
	light.transforms.push_back( t );
	light.colours.push_back( Vector3f(1.0, 0.0, 0.0) );
	light.types.push_back( Renderer::LightType_directional );

	physics.capture(object.transforms, renderer.eye);
}

void App::step(Input& input, double dt_)
{
	dt = dt_;
	player_movement = Vector2f::Zero();
	player_jump = false;

	for(auto i = input.keys.begin(); i != input.keys.end(); i++)
	{
		if ((*i).up) held_keys.erase( (*i).key );
		else held_keys.insert( (*i).key );
		on_key( (*i).key, (*i).up );
	}

	for(auto i = held_keys.begin(); i != held_keys.end(); i++)
	{
		on_held_key( *i );
	}

	if (player_movement.x() != 0 || player_movement.y() != 0)
	{
		player_movement.normalize();
	}

	float yaw = float(renderer.camera.yaw / 180 * M_PI);
	Matrix2f rot;
	rot << cos(yaw), sin(yaw), -sin(yaw), cos(yaw);
	physics.control_player( -rot * player_movement, player_jump );

	on_mouse(input.mouse.x, input.mouse.y);

	physics.step(dt);
	renderer.render();
}

} // namespace Devora