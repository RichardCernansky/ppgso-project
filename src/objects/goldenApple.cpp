#include "goldenApple.h"

#include "apple.h"
#include "src/generator.h"
#include "../objects/stone.h"

// Static resources (shared among all instances of Apple)
std::unique_ptr<ppgso::Mesh> GoldenApple::mesh;
std::unique_ptr<ppgso::Texture> GoldenApple::texture;

// Constructor
GoldenApple::GoldenApple() {
    // Generate a random position with each axis offset by a random value in the range [-1, 1]
    float baseX = -0.0f, baseY = 3.0f, baseZ = -0.0f;
    constructorPosition = glm::vec3{
        baseX + randomFloat(-2.0f, 2.0f),  // Random offset from baseX
        baseY + randomFloat(-2.0f, 2.0f),  // Random offset from baseY
        baseZ + randomFloat(-2.0f, 2.0f)   // Random offset from baseZ
    };
    position = constructorPosition;

    // Load texture once and reuse across all instances
    if (!texture) {
        auto image = ppgso::image::loadBMP("golden_apple.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: apple.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

    // Load mesh once and reuse across all instances
    if (!mesh) {
        mesh = std::make_unique<ppgso::Mesh>("apple.obj");
        if (!mesh) {
            std::cerr << "Failed to load mesh: apple.obj" << std::endl;
        }
    }
}


bool GoldenApple::update(float dTime, Scene &scene) {
    return true;
}

bool GoldenApple::update_child(float dTime, Scene &scene, glm::mat4 ParentModelMatrix) {
    dt = dTime;

    if (isFalling) {
        // Apply gravity and wind forces
        glm::vec3 gravity_force = glm::vec3(0.0f, -gravity * mass, 0.0f);
        glm::vec3 total_force = gravity_force + wind_force;

        // Update acceleration, velocity, and position
        acceleration = total_force / mass;
        velocity += acceleration * dt;
        position += velocity * dt;

        // Update the model matrix to get the current world position
        glm::mat4 localModelMatrix = glm::translate(glm::mat4(1.0f), position);
        localModelMatrix = glm::scale(localModelMatrix, scale);
        modelMatrix = ParentModelMatrix * localModelMatrix;

        glm::vec4 worldPos = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 globalPosition = glm::vec3(worldPos);


        for (auto &object : scene.objects) {
            Ground* ground = dynamic_cast<Ground*>(object.get());
            if (ground) {
                for (auto &obj : ground->children) {
                    AppleTree* tree = dynamic_cast<AppleTree*>(obj.get());
                    if (tree) {
                         for (auto &obje : tree->children) {
                            Stone* stone = dynamic_cast<Stone*>(obje.get());
                            if (stone) {
                                // Calculate the vector between apple and stone
                                glm::vec3 diff = position - stone->position;
                                float distance = glm::length(diff);
                                float combinedRadius = radius + stone->radius;


                                // Check for collision
                                if (distance < combinedRadius) {
                                    // Normalize the collision normal
                                    glm::vec3 collisionNormal = glm::normalize(diff);

                                    // Calculate relative velocity
                                    glm::vec3 relativeVelocity = velocity*0.2f;

                                    // Calculate velocity along the normal
                                    float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

                                    // If velocities are separating, do not resolve
                                    if (velocityAlongNormal > 0)
                                        continue;

                                    // Calculate restitution
                                    float e = restitution;

                                    // Calculate impulse scalar
                                    float j = -(1.0f + e) * velocityAlongNormal;;

                                    // Apply impulse
                                    glm::vec3 impulse = j * collisionNormal;
                                    velocity += impulse / mass;
                                    // Positional correction to prevent sinking
                                    const float percent = 0.8f; // Penetration percentage to correct
                                    const float slop = 0.01f;   // Penetration allowance
                                    float penetration = combinedRadius - distance;
                                    float correctionMagnitude = std::max(penetration - slop, 0.0f) / (1.0f / mass) * percent;
                                    glm::vec3 correction = correctionMagnitude * collisionNormal;
                                    position += correction / mass;;

                                    // Optional: Apply friction
                                    glm::vec3 tangent = relativeVelocity - (glm::dot(relativeVelocity, collisionNormal) * collisionNormal);
                                    if (glm::length(tangent) > 0.0001f)
                                        tangent = glm::normalize(tangent);

                                    // Calculate friction scalar
                                    float mu = 0.5f; // Friction coefficient
                                    float jt = -glm::dot(relativeVelocity, tangent);

                                    // Clamp friction impulse
                                    float frictionImpulseMagnitude = (std::abs(jt) < j * mu) ? jt : -j * mu;
                                    glm::vec3 frictionImpulse = frictionImpulseMagnitude * tangent;

                                    // Apply friction impulse
                                    velocity += frictionImpulse / mass;
                                }
                            }
                        }

                    }
                }
            }
        }

        // Ground collision handling
        if (position.y < 0.1f) {
            position.y = 0.1f + std::abs(velocity.y * 0.01f);
            float variedRestitution = restitution * randomFloat(0.9f, 1.1f);
            velocity.y = -velocity.y * variedRestitution;
            float dampingFactor = 0.98f;
            velocity *= dampingFactor;

            if (std::abs(velocity.y) < 0.1f) {
                isFalling = false;
                respawnTime = randomFloat(1.0f, 2.0f);  // Random delay between 1 and 2 seconds
                elapsedTime = 0.0f;  // Reset elapsed time for the respawn delay

                position = constructorPosition;
                velocity = glm::vec3(0.0f);
                acceleration = glm::vec3(0.0f);
            }
        }

    } else {
        // Waiting for respawn
        elapsedTime += dt;

        // If enough time has passed, reset apple's position and start falling again
        if (elapsedTime >= respawnTime) {
            isFalling = true;  // Set back to falling state
        }
    }

    // Update the model matrix after position changes
    glm::mat4 localModelMatrix = glm::translate(glm::mat4(1.0f), position);
    localModelMatrix = glm::scale(localModelMatrix, scale);
    modelMatrix = ParentModelMatrix * localModelMatrix;

    return true;
}


void GoldenApple::render(Scene &scene) {
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
    if (mesh) {
        mesh->render();
    }
}
