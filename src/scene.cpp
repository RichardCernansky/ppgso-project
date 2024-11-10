#include <list>
#include <memory>
#include "renderable.h"
#include "camera.h"
#ifndef SCENE
#define SCENE


class Scene
{
public:
	// list of objects
	std::list<std::unique_ptr<Renderable>> objects;

	// camera
	std::unique_ptr<Camera> camera;

	// shader
	std::unique_ptr<ppgso::Shader> shader;

	// lights
	std::vector<glm::vec3> light_positions;
	int LIGHT_COUNT = 3;

	float Dtime;

	// update function
	void update(float time)
	{
		Dtime = time;
		auto i = std::begin(objects);
		while (i != std::end(objects))
		{
			// Update object and remove from list if needed
			auto obj = i->get();
			if (!obj->update(time, *this))
				i = objects.erase(i);
			else
				++i;
		}

		camera->update(Dtime);
	}

	// render function
	void render()
	{
		for (auto &object : objects)
		{
			object->render(*this);
		}
	}
};

#endif