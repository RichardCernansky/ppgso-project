#include "goldenApple.h"

#include "apple.h"
#include "src/generator.h"
#include "../objects/stone.h"

std::unique_ptr<ppgso::Mesh> GoldenApple::mesh;
std::unique_ptr<ppgso::Texture> GoldenApple::texture;

GoldenApple::GoldenApple() {
    float baseX = -0.0f, baseY = 3.0f, baseZ = -0.0f;
    constructorPosition = glm::vec3{
        baseX + randomFloat(-2.0f, 2.0f),
        baseY + randomFloat(-2.0f, 2.0f),
        baseZ + randomFloat(-2.0f, 2.0f)
    };
    position = constructorPosition;

    if (!texture) {
        auto image = ppgso::image::loadBMP("golden_apple.bmp");
        if (image.width == 0 || image.height == 0) {
            std::cerr << "Failed to load texture: apple.bmp" << std::endl;
            return;
        }
        texture = std::make_unique<ppgso::Texture>(std::move(image));
    }

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
        glm::vec3 gravity_force = glm::vec3(0.0f, -gravity * mass, 0.0f);
        glm::vec3 total_force = gravity_force + wind_force;

        acceleration = total_force / mass;
        velocity += acceleration * dt;
        position += velocity * dt;

        glm::mat4 localModelMatrix = glm::translate(glm::mat4(1.0f), position);
        localModelMatrix = glm::scale(localModelMatrix, scale);
        modelMatrix = ParentModelMatrix * localModelMatrix;

        glm::vec4 worldPos = modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 globalPosition = glm::vec3(worldPos);


        //traverse the scene graph
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

        if (position.y < 0.1f) {
            position.y = 0.1f + std::abs(velocity.y * 0.01f);
            float variedRestitution = restitution * randomFloat(0.9f, 1.1f);
            velocity.y = -velocity.y * variedRestitution;
            float dampingFactor = 0.98f;
            velocity *= dampingFactor;

            if (std::abs(velocity.y) < 0.1f) {
                isFalling = false;
                respawnTime = randomFloat(1.0f, 2.0f);
                elapsedTime = 0.0f;

                position = constructorPosition;
                velocity = glm::vec3(0.0f);
                acceleration = glm::vec3(0.0f);
            }
        }

    } else {
        elapsedTime += dt;

        if (elapsedTime >= respawnTime) {
            isFalling = true;
        }
    }

    glm::mat4 localModelMatrix = glm::translate(glm::mat4(1.0f), position);
    localModelMatrix = glm::scale(localModelMatrix, scale);
    modelMatrix = ParentModelMatrix * localModelMatrix;

    return true;
}


void GoldenApple::render(Scene &scene) {
    scene.shader->use();

    scene.shader->setUniform("ModelMatrix", modelMatrix);
    scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
    if (texture) {
        scene.shader->setUniform("Texture", *texture);
    }

    if (mesh) {
        mesh->render();
    }
}
