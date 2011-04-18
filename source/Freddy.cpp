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

void Freddy::on_mouse(long dx, long dy)
{
	renderer.camera.yaw += dx * settings.mouse_sens;
	renderer.camera.pitch += dy * settings.mouse_sens;
}

Freddy::Freddy(Settings settings_) :
	renderer(settings_.r)
{
	settings = settings_;

	Object object = { Matrix4f::Identity(), renderer.read_geom("geometry//sphere.geom")};
	object.world.col(3) = Vector4f(0.0, 0.0, 1.0, 1.0);
	objects.push_back(object);

	Object object2 = { Matrix4f::Identity(), renderer.read_geom("geometry//plane.geom")};
	objects.push_back(object2);

	Light light = { Matrix4f::Identity(), Vector3f(1.0, 1.0, 1.0) };
	light.world.col(3) = Vector4f(1.0, 2.0, 1.0, 1.0);
	lights.push_back(light);
}

void Freddy::step(Input& input, double dt)
{
	for(auto i = input.keys.begin(); i != input.keys.end(); i++)
	{
		on_key( (*i).key, (*i).up );
	}

	on_mouse(input.mouse.x, input.mouse.y);

	renderer.render(objects, lights);
}