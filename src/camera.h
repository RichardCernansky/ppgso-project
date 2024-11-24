#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <ppgso/ppgso.h>
#include <iostream>

#ifndef CAMERA
#define CAMERA

class Camera
{
public:
    glm::mat4 perspective;
    glm::mat4 viewMatrix{1.0f};

    glm::vec3 position{0.01f, 0.2f, 0.01f};
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

    struct Keyframe {
        glm::vec3 position;
        glm::vec3 eulerAngles;
        float duration;
    };

    size_t currentKeyframeIndex = 0;
    float currentTime = 0.0f;

    std::vector<Keyframe> keyframes = {
        //hore
        //{{0.0f, 15.0f, 0.0f}, {-90.0f, 0.0f, 0.0f}, 2.0f},
        //apples on a tree
        {{0.0f, 10.0f, 0.0f}, {-90.0f, 0.0f, 0.0f}, 2.0f},

        {{0.0f, 1.0f, -8.0f}, {0.0f, 0.0f, 0.0f}, 15.0f},
        //fire
        {{4.0f, 2.0f, -12.0f}, {0.0f, -180.0f, 0.0f}, 15.0f},
        {{0.0f, 2.0f, -3.0f}, {0.0f, -180.0f, 0.0f}, 15.0f},
        //bee
        {{1.0f, 1.5f, 2.0f}, {0.0f, 80.0f, 0.0f}, 15.0f},
        //grass
        {{3.0f, 1.0f, -3.0f}, {0.0f, 90.0f, 0.0f}, 15.5f},
        //rock
        {{13.0f, 2.0f, -6.0f}, {0.0f, 90.0f, 0.0f}, 15.0f},
        //rock
        {{10.0f, 0.5f, -9.0f}, {0.0f, 90.0f, 0.0f}, 10.0f},
        //back home
        {{0.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, 10.0f},
        {{0.0f, 4.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, 10.0f},
    };

    // Constructor
    Camera(float fov = 45.0f, float ratio = 1.0f, float near = 0.1f, float far = 10.0f)
    {
        perspective = glm::perspective(glm::radians(fov), ratio, near, far);
    }

    glm::vec3 catmullRomInterpolate(const glm::vec3& p0, const glm::vec3& p1,
                                    const glm::vec3& p2, const glm::vec3& p3, float t) {
        float t2 = t * t;
        float t3 = t2 * t;

        return 0.5f * (
            (2.0f * p1) +
            (-p0 + p2) * t +
            (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
            (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
        );
    }


    void update(float deltaTime) {
        if (currentKeyframeIndex >= keyframes.size() - 1) {
            // Optionally loop back to the first keyframe for continuous motion
            // currentKeyframeIndex = 0;
            // currentTime = 0.0f;

            // Or, keep the camera at the last keyframe
            Keyframe& lastFrame = keyframes.back();
            position = lastFrame.position;
            glm::vec3 eulerAnglesRad = glm::radians(lastFrame.eulerAngles);
            glm::quat orientation = glm::quat(eulerAnglesRad);
            front = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
            viewMatrix = glm::lookAt(position, position + front, up);
            return;
        }

        // Ensure there are enough keyframes for Catmull-Rom (need p0 to p3)
        if (currentKeyframeIndex >= keyframes.size() - 2) {
            // Optionally, handle the end of the keyframe sequence
            return;
        }

        Keyframe& p0 = keyframes[currentKeyframeIndex == 0 ? 0 : currentKeyframeIndex - 1];
        Keyframe& p1 = keyframes[currentKeyframeIndex];
        Keyframe& p2 = keyframes[currentKeyframeIndex + 1];
        Keyframe& p3 = keyframes[currentKeyframeIndex + 2];

        // Update current time
        currentTime += deltaTime;

        // Calculate interpolation factor t
        float t = currentTime / p1.duration;
        t = glm::clamp(t, 0.0f, 1.0f);

        // Apply Catmull-Rom interpolation for position
        position = catmullRomInterpolate(p0.position, p1.position, p2.position, p3.position, t);

        // Interpolate orientation using SLERP
        glm::vec3 startEulerRad = glm::radians(p1.eulerAngles);
        glm::vec3 endEulerRad = glm::radians(p2.eulerAngles);

        glm::quat startQuat = glm::quat(startEulerRad);
        glm::quat endQuat = glm::quat(endEulerRad);

        glm::quat interpolatedQuat = glm::slerp(startQuat, endQuat, t);
        front = interpolatedQuat * glm::vec3(0.0f, 0.0f, -1.0f);

        // Update view matrix
        viewMatrix = glm::lookAt(position, position + front, up);

        // If duration reached, move to next keyframe
        if (currentTime >= p1.duration) {
            currentTime = 0.0f;
            currentKeyframeIndex++;
        }
    }

};

#endif
