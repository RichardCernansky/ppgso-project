#include "wolf.h"

#include "pig.h"
#include "src/generator.h"
#include "src/globals.h"

// Static resources (shared among all instances of Wolf)
std::unique_ptr<ppgso::Mesh> Wolf::mesh;
std::unique_ptr<ppgso::Texture> Wolf::texture;

// Constructor
Wolf::Wolf(){
    // Generate a random position
    position = glm::vec3{2, 0, 0};

    // Load texture once and reuse across all instances
    if (!texture) {
        auto image = ppgso::image::loadBMP("wolf.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: wolf.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    // Load mesh once and reuse across all instances
    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("wolf.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: wolf.obj" << std::endl;
        }
    }
}

int Wolf::findClosestPigIndex(const Scene &scene) const {
    float minDistance = std::numeric_limits<float>::max();
    int closestIndex = -1;

    // Loop through all objects in the scene
    int idx = 0;
    for (const auto& object : scene.objects) {

        // Check if the unique_ptr is pointing to a Pig using typeid
        if (typeid(*object) == typeid(Pig)) {
            // Calculate distance using the getter method
            float distance = glm::distance(this->getPosition(), object->getPosition());

            // Check if this pig is the closest so far
            if (distance < minDistance) {
                minDistance = distance;
                closestIndex = idx; // Store the index of the closest Pig
            }
        }
        ++idx;
    }

    return closestIndex;
}


bool Wolf::update(float dTime, Scene &scene) {
    // Find the index of the closest pig
    int closestPigIndex = findClosestPigIndex(scene);

    // Check if a valid pig index was found
    if (closestPigIndex != -1) {
        // Get an iterator to the object at the closestPigIndex in the list
        auto it = scene.objects.begin();
        std::advance(it, closestPigIndex);  // Move the iterator to the closestPigIndex position

        // Get the position of the closest pig
        const auto& closestPig = *it;
        glm::vec3 pig_pos = closestPig->getPosition();

        // Calculate the direction vector towards the pig (in the XZ plane)
        glm::vec3 direction = glm::normalize(glm::vec3(pig_pos.x - position.x, 0.0f, pig_pos.z - position.z));

        // Calculate the distance between the wolf and the pig
        float distanceToPig = glm::distance(glm::vec3(position.x, 0.0f, position.z), glm::vec3(pig_pos.x, 0.0f, pig_pos.z));

        // Calculate the angle between the wolf's current forward direction and the direction to the pig
        glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f); // Assuming the wolf's initial forward direction is along Z-axis
        float angle = atan2(direction.x, direction.z); // Angle in radians around the Y-axis

        // Construct a rotation matrix around the Y-axis
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

        // Update the model matrix with translation, rotation, and scale
        modelMatrix = glm::mat4(1.0f); // Reset model matrix
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= rotationMatrix; // Apply rotation around the Y-axis
        modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3{-1, 0, 0});
        modelMatrix = glm::scale(modelMatrix, scale);

        // Determine the speed of the wolf based on its distance to the pig
        float maxSpeed = 5.0f;  // Maximum speed the wolf can reach
        float minDistanceToStartSlowing = 4.0f;  // Distance at which the wolf starts slowing down
        float speed;

        if (distanceToPig > minDistanceToStartSlowing) {
            // Accelerate towards the maximum speed
            speed = glm::min(maxSpeed, speed + (dTime * 0.001f)); // Accelerate smoothly to the max speed
        } else {
            // Decelerate as the wolf gets closer to the pig
            float decelerationFactor = distanceToPig / minDistanceToStartSlowing * 0.5; // Factor goes from 1 to 0
            speed = glm::max(0.5f, decelerationFactor * maxSpeed); // Decelerate smoothly
        }

        // Update the wolf’s position towards the pig
        velocity = direction * speed;
        position += velocity * dTime;
    }

    // Other update logic (e.g., collisions, animations, etc.)

    return true;
}


void Wolf::render(Scene &scene) {
    // Use the shadow projection matrix
    glm::mat4 shadowMatrix = calculateShadowMatrix(moonLight_position, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

    // Render the shadow
    scene.colorShader->use();
    scene.colorShader->setUniform("ModelMatrix", shadowMatrix * modelMatrix);
    scene.colorShader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.colorShader->setUniform("ProjectionMatrix", scene.camera->perspective);

    // Render the pig's shadow as a black silhouette
    glDisable(GL_DEPTH_TEST); // Prevent z-fighting
    scene.colorShader->setUniform("Color", glm::vec3(0.0f, 0.0f, 0.0f)); // Black shadow
    mesh->render();
    glEnable(GL_DEPTH_TEST);
    // Use the shader program
    scene.shader->use();

    // Set shader uniforms
    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    if (texture) {
        scene.shader->setUniform("Texture", *texture);
    }

    // Render the mesh
    mesh->render();

}