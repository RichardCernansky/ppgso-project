#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/euler_angles.hpp>
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
        {{1.0f, 0.5f, 2.0f}, {0.0f, 0.0f, 0.0f}, 1.0f},
        {{0.0f, 1.0f, 1.5f}, {0.0f, 90.0f, 0.0f}, 1.5f},
        {{0.0f, 1.0f, 0.0f}, {0.0f, 180.0f, 0.0f}, 2.0f},
        {{1.0f, 1.5f, 0.5f}, {0.0f, 80.0f, 0.0f}, 1.2f},
        {{2.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, 1.0f}
    };

    // Constructor
    Camera(float fov = 45.0f, float ratio = 1.0f, float near = 0.1f, float far = 10.0f)
    {
        perspective = glm::perspective(glm::radians(fov), ratio, near, far);
    }

    void update(float deltaTime) {
        if (currentKeyframeIndex >= keyframes.size() - 1) {
            Keyframe& lastFrame = keyframes.back();
            position = lastFrame.position;
            glm::vec3 eulerAnglesRad = glm::radians(lastFrame.eulerAngles);
            glm::quat orientation = glm::quat(eulerAnglesRad);
            front = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
            viewMatrix = glm::lookAt(position, position + front, up);
            return;
        }

        Keyframe& startFrame = keyframes[currentKeyframeIndex];
        Keyframe& endFrame = keyframes[currentKeyframeIndex + 1];

        // Update current time
        currentTime += deltaTime;

        // Calculate interpolation factor t
        float t = currentTime / startFrame.duration;
        t = glm::clamp(t, 0.0f, 1.0f);

        // Interpolate position
        position = glm::mix(startFrame.position, endFrame.position, t);

        // Convert Euler angles to radians
        glm::vec3 startEulerRad = glm::radians(startFrame.eulerAngles);
        glm::vec3 endEulerRad = glm::radians(endFrame.eulerAngles);

        // Convert Euler angles to quaternions
        glm::quat startQuat = glm::quat(startEulerRad);
        glm::quat endQuat = glm::quat(endEulerRad);

        // Interpolate quaternions
        glm::quat interpolatedQuat = glm::slerp(startQuat, endQuat, t);

        // Update front vector
        front = interpolatedQuat * glm::vec3(0.0f, 0.0f, -1.0f);

        // Update view matrix
        viewMatrix = glm::lookAt(position, position + front, up);

        // If duration reached, move to next keyframe
        if (currentTime >= startFrame.duration) {
            currentTime = 0.0f;
            currentKeyframeIndex++;
        }
    }
};

#endif
