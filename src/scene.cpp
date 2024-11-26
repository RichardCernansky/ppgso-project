#include <list>
#include <memory>
#include "renderable.h"
#include "camera.h"
#include "generator.h"



#ifndef SCENE
#define SCENE


class Scene {
public:
	// list of objects
	std::list<std::unique_ptr<Renderable>> objects;

	// camerax
	std::unique_ptr<Camera> camera;

	// shader
	std::unique_ptr<ppgso::Shader> shader;
	std::unique_ptr<ppgso::Shader> colorShader;

	// lights
	std::vector<glm::vec3> light_positions;
	int LIGHT_COUNT = 1;

	float Dtime;

	void update(float time)
	{
		set_up_lights(shader->getProgram());
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


	//check if the object is transparent
	template <typename T>
	auto checkTransparency(T* object) -> decltype(object->isTransparent(), bool()) {
		return object->isTransparent();
	}

	template <typename T>
	bool checkTransparency(...) {
		return false; // Default to opaque
	}

	void render() {
		//list to hold objects
		std::list<Renderable*> transparentObjects;
		std::list<Renderable*> opaqueObjects;

		// traverse the tree
		auto categorizeObjects = [&](Renderable* object, auto& self) -> void {
			if (!object) return;

			if (checkTransparency(object)) {
				transparentObjects.push_back(object);
			} else {
				opaqueObjects.push_back(object);
			}

			// recursively call the function
			const auto& children = object->getChildren();
			for (const auto& child : children) {
				self(child.get(), self); // Recurse into children
			}
		};

		for (const auto& object : objects) {
			categorizeObjects(object.get(), categorizeObjects);
		}

		// render opaque objects first
		for (const auto& object : opaqueObjects) {
			object->render(*this);
		}

		// sort transparent objects
		glm::vec3 cameraPosition = camera->position;
		transparentObjects.sort([cameraPosition](const Renderable* a, const Renderable* b) {
			return a->calculateDepthFromCamera(cameraPosition) > b->calculateDepthFromCamera(cameraPosition);
		});

		// render transparent objects
		for (const auto& object : transparentObjects) {
			object->render(*this);
		}
	};
};

#endif