#ifndef __Freddy_h__
#define __Freddy_h__

#include <list>
#include <memory>

#include "Matrix.h"
#include "Renderer.h"
#include "Physics.h"
#include "Input.h"

class Freddy
{
public:
	class Quit {};

	struct Settings
	{
		Renderer::Settings r;
		float mouse_sens;
	};

private:
	Renderer renderer;
	Settings settings;

	typedef Renderer::Object Object;
	typedef Renderer::Light Light;
	std::vector<Object, Eigen::aligned_allocator<Object>> objects;
	std::vector<Light, Eigen::aligned_allocator<Light>> lights;

	void on_key(int key, bool up);
	void on_mouse(long dx, long dy);
public:
	Freddy(Settings settings_);
	void step(Input& input, double dt);

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif