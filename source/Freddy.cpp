#include "Freddy.h"
#include "Ok.h"

void Freddy::on_key(int key, bool up)
{
	if(!up)
	{
		switch(key)
		{
		case 27: // ESC
			throw Quit();
			break;
		}
	}
}

void Freddy::on_held_key(int key)
{

}


void Freddy::on_mouse(long dx, long dy)
{
	renderer.camera.yaw += dx * settings.mouse_sens;
	renderer.camera.pitch += dy * settings.mouse_sens;
}

Freddy::Freddy(Settings settings_) :
	renderer(object, light, settings_.r)
{
	settings = settings_;

	auto plane = renderer.read_geom("geometry//plane.geom");
	auto sphere = renderer.read_geom("geometry//sphere.geom");

	object.transforms.push_back( Matrix4f::Identity() );
	object.geometries.push_back( plane );

	Matrix4f t = Matrix4f::Identity();
	for (int i = 0; i < 10; i++)
	{
		for (int j = -2; j <= 2; j++)
		{
			t.col(3) = Vector4f(-10.0f + j % 2, j * 2.0f + i % 2, i * 2.0f + 1.0f, 1.0f);
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
	light.colours.push_back( Vector3f(0.0, 0.0, 0.5) );
	light.types.push_back( Renderer::LightType_directional );

	t = Matrix4f::Identity();
	t.col(3) = Vector4f(0.0, -7.0, 1.0, 1.0);
	light.transforms.push_back( t );
	light.colours.push_back( Vector3f(0.0, 0.0, 0.5) );
	light.types.push_back( Renderer::LightType_directional );

	physics.capture(object.transforms, renderer.eye);
}

void Freddy::step(Input& input, double dt_)
{
	dt = dt_;
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

	on_mouse(input.mouse.x, input.mouse.y);

	physics.step(dt);
	renderer.render();
}