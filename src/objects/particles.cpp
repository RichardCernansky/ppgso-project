// //
// // Created by Bruno Kristián on 23/10/2024.
// //
// #include "particles.h"
// std::unique_ptr<ppgso::Mesh> Particles::mesh;
// std::unique_ptr<ppgso::Texture> Particles::texture;
//
// // Constructor
// Particles::Particles(){
//
//     if (!texture) {
//         auto image = ppgso::image::loadBMP("grey.bmp");
//         if (image.width == 0 || image.height == 0) {
//             std::cerr << "Failed to load texture: grey.bmp" << std::endl;
//             return;
//         }
//         texture = std::make_unique<ppgso::Texture>(std::move(image));
//     }
//
//     if (!mesh) {
//         mesh = std::make_unique<ppgso::Mesh>("sphere.obj");  // Load the boar model
//         if (!mesh) {
//             std::cerr << "Failed to load mesh: sphere.obj" << std::endl;
//         }
//     }
// }
//
// bool Particles::update(float dTime, Scene &scene) {
//     return true;
// }
//
// // Update method
// bool Particles::update_child(float dTime, Scene &scene, glm::mat4 ParetModelMatrix) {
//
//     modelMatrix = glm::mat4{1.0f};
//     modelMatrix = glm::translate(modelMatrix, position);
//     modelMatrix = glm::scale(modelMatrix, scale);
//
//     // update position
//     if (position.x < 0 || position.x > 0)
//         speed.x *= -1;
//     if (position.y < 0.5 || position.y > 1)
//         speed.y *= -1;
//     if (position.z < -2 || position.z > 1)
//         speed.z *= -1;
//
//     position += speed * dTime;
//
//     // update color
//     float t = glfwGetTime();
//     color = basecolor * (float)((cos(t) + 1) / 2.0f + 0.5);
//
//     // iterate through flies
//     auto i = std::begin(flies);
//     while (i != std::end(flies))
//     {
//         // update
//         auto f = i->get();
//
//         f->position = position;
//         f->color = color;
//
//         if (!f->update(dTime, scene))
//         {
//             // remove and spawn a new one
//             i = flies.erase(i);
//             spawnAFireFly();
//         }
//         else
//             ++i;
//     }
//
//     return true;
// }
//
// void Particles::render(Scene &scene) {
//     float transparency = 0.5f;
//
//     scene.shader->use();
//
//     scene.shader->setUniform("ModelMatrix", modelMatrix);
//     scene.shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
//     scene.shader->setUniform("ProjectionMatrix", scene.camera->perspective);
//     scene.shader->setUniform("Texture", *texture);
//     scene.shader->setUniform("Transparency", transparency);
//
//     glEnable(GL_BLEND);
//     glEnable(GL_DEPTH_TEST);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//     glDepthMask(GL_FALSE);
//
//     // Render the mesh
//     mesh->render();
//
//     glDepthMask(GL_TRUE);
//     glDisable(GL_BLEND);
// }
