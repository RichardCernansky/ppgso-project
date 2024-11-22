
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

	// Default implementation of shadow matrix calculation
	static glm::mat4 calculateShadowMatrix(const glm::vec3& lightPosition, const glm::vec4& plane) {
    	glm::mat4 shadowMatrix(0.0f);
    	float dot = plane.x * lightPosition.x + plane.y * lightPosition.y + plane.z * lightPosition.z + plane.w;

    	shadowMatrix[0][0] = dot - lightPosition.x * plane.x;
    	shadowMatrix[1][0] = -lightPosition.x * plane.y;
    	shadowMatrix[2][0] = -lightPosition.x * plane.z;
    	shadowMatrix[3][0] = -lightPosition.x * plane.w;

    	shadowMatrix[0][1] = -lightPosition.y * plane.x;
    	shadowMatrix[1][1] = dot - lightPosition.y * plane.y;
    	shadowMatrix[2][1] = -lightPosition.y * plane.z;
    	shadowMatrix[3][1] = -lightPosition.y * plane.w;

    	shadowMatrix[0][2] = -lightPosition.z * plane.x;
    	shadowMatrix[1][2] = -lightPosition.z * plane.y;
    	shadowMatrix[2][2] = dot - lightPosition.z * plane.z;
    	shadowMatrix[3][2] = -lightPosition.z * plane.w;

    	shadowMatrix[0][3] = -plane.x;
    	shadowMatrix[1][3] = -plane.y;
    	shadowMatrix[2][3] = -plane.z;
    	shadowMatrix[3][3] = dot;

    	return shadowMatrix;
    }

	const glm::vec3& getPosition() const {
    	return position;
    }

	virtual float calculateDepthFromCamera(const glm::vec3& cameraPosition) const {
    	return 0.0f;
    }



};
#endif
