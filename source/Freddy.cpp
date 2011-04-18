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
		case 87: // W
			renderer.eye += Vector3f(1.0, 0.0, 0.0) * float(dt) * settings.camera_speed;
			break;
		case 83: // S
			renderer.eye -= Vector3f(1.0, 0.0, 0.0) * float(dt) * settings.camera_speed;
			break;
		case 65: // A
			renderer.eye += Vector3f(0.0, 0.0, 1.0) * float(dt) * settings.camera_speed;
			break;
		case 68: // D
			renderer.eye -= Vector3f(0.0, 0.0, 1.0) * float(dt) * settings.camera_speed;
			break;
		}
	}
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

	Matrix4f t;
	for(int i = 0; i < 8; i++)
	{
		t.col(3)[2] = i * 2.0f;
		for(int j = -4; j < 5; j++)
		{
			t.col(3)[1] = float(i % 2) + j * 2.0f;
			t.col(3)[0] = -10 + float(i % 2);
			object.transforms.push_back( t );
			object.geometries.push_back( sphere );
		}
	};

	t = Matrix4f::Identity();
	t.col(3) = Vector4f(-5.0, -3.0, 1.0, 1.0);
	light.transforms.push_back( t );
	light.colours.push_back( Vector3f(1.0, 0.0, 0.0) );

	t.col(3) = Vector4f(-5.0, 0.0, 1.0, 1.0);
	light.transforms.push_back( t );
	light.colours.push_back( Vector3f(0.0, 1.0, 0.0) );

	t.col(3) = Vector4f(-5.0, 3.0, 1.0, 1.0);
	light.transforms.push_back( t );
	light.colours.push_back( Vector3f(0.0, 0.0, 1.0) );

	physics.capture(object.transforms);
}

void Freddy::step(Input& input, double dt_)
{
	dt = dt_;
	for(auto i = input.keys.begin(); i != input.keys.end(); i++)
	{
		on_key( (*i).key, (*i).up );
	}

	on_mouse(input.mouse.x, input.mouse.y);

	physics.step(dt);
	renderer.render();
}