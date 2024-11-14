
#include "camera.h"

#ifndef RENDERABLE
#define RENDERABLE
class Scene;

class Renderable
{
public:
	// Virtual destructor is needed for abstract interfaces
	virtual ~Renderable() = default;

	glm::vec3 position;
	/// Render the object
	/// \param camera - Camera to use for rendering
	virtual void render(Scene &scene) = 0;

	/// Update the object. Useful for specifing animation and behaviour.
	/// \param dTime - Time delta
	/// \param scene - Scene reference
	/// \return - Return true to keep object in scene
	virtual bool update(float dTime, Scene &scene) = 0;

    virtual bool update_child(float d, Scene &scene, glm::mat4 ParetModelMatrix) {
        return false;
    }

	virtual bool isTransparent() const { return false; }

	const glm::vec3& getPosition() const {
    	return position;
    }

	virtual float calculateDepthFromCamera(const glm::vec3& cameraPosition) const {
    	return 0.0f;
    }



};
#endif
