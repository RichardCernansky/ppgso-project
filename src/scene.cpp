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

	// update function
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

	template <typename T>
	auto checkTransparency(T* object) -> decltype(object->isTransparent(), bool()) {
		return object->isTransparent();
	}

	// Fallback helper function if isTransparent() does not exist
	template <typename T>
	bool checkTransparency(...) {
		return false; // Default to opaque
	}

	// render function
	void render() {
		// Temporary lists holding raw pointers to Renderable objects
		std::list<Renderable*> transparentObjects;
		std::list<Renderable*> opaqueObjects;

		std::cout << "Number of top-level objects: " << objects.size() << std::endl;

		// Helper function to recursively traverse the tree
		auto categorizeObjects = [&](Renderable* object, auto& self) -> void {
			if (!object) return;

			// Categorize the current object
			if (checkTransparency(object)) {
				transparentObjects.push_back(object);
			} else {
				opaqueObjects.push_back(object);
			}

			// Recursively process children
			const auto& children = object->getChildren();
			std::cout << "Children count: " << children.size() << std::endl;
			for (const auto& child : children) {
				self(child.get(), self); // Recurse into children
			}
		};

		// Start traversal from top-level objects
		for (const auto& object : objects) {
			categorizeObjects(object.get(), categorizeObjects);
		}

		// Render opaque objects first
		for (const auto& object : opaqueObjects) {
			object->render(*this);
		}

		// Sort transparent objects by depth from the camera (furthest first)
		glm::vec3 cameraPosition = camera->position;
		transparentObjects.sort([cameraPosition](const Renderable* a, const Renderable* b) {
			return a->calculateDepthFromCamera(cameraPosition) > b->calculateDepthFromCamera(cameraPosition);
		});

		// Render transparent objects in sorted order
		for (const auto& object : transparentObjects) {
			object->render(*this);
		}
	};
};

#endif