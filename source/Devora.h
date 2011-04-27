#ifndef __Devora_h__
#define __Devora_h__

#include <list>
#include <memory>
#include <set>

#include "Matrix.h"
#include "Renderer.h"
#include "Physics.h"
#include "Input.h"

class Devora
{
public:
	class Quit {};

	struct Settings
	{
		Renderer::Settings r;
		float mouse_sens;
		float camera_speed;
	};

private:
	Renderer renderer;
	Physics physics;
	Settings settings;
	std::set<int> held_keys;
	double dt;
	Vector2f player_movement;
	bool player_jump;

	void on_held_key(int key);
	void on_key(int key, bool up);
	void on_mouse(long dx, long dy);
public:
	Renderer::ObjectData object;
	Renderer::LightData light;

	Devora(Settings settings_);
	void step(Input& input, double dt_);

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif