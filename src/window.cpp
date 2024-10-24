//system libs
#include <iostream>
#include <vector>
#include <list>
//gl libs
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <ppgso/ppgso.h>
//shaders
#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>
//my files
#include "scene.cpp"
#include "objects/ground.cpp"
#include "objects/tree.h"
#include "generator.h"
#include "objects/apple.h"
#include "objects/gas.h"
#include "objects/horseFly.h"
#include "src/objects/pig.h"
#include "objects/AppleTree.h"
#include "objects/fire.h"
#include "objects/goldenApple.h"
#include "objects/smoke.h"
#include "objects/particles.h"


class ProjectWindow : public ppgso::Window
{
private:
	Scene scene;

	// Objects to render the framebuffer on to
	// ppgso::Shader quadShader = {convolution_vert_glsl, convolution_frag_glsl};
	ppgso::Shader quadShader = {texture_vert_glsl, texture_frag_glsl};
	ppgso::Mesh quadMesh = {"quad.obj"};
	ppgso::Texture quadTexture = {1024, 1024};

	// OpenGL object ids for framebuffer and render buffer
	GLuint fbo = 1;
	GLuint rbo = 1;

	void initBase() {
		// Camera setup
		auto camera = std::make_unique<Camera>(100.0f, (float)width / (float)height, 0.1f, 100.0f);
		scene.camera = std::move(camera);

		// Add ground object to the scene
		scene.objects.push_back(std::make_unique<Ground>());

		auto tree = std::make_unique<Tree>(); //generate texture
		for (int i = 0 ; i < 200; i++) { //generate and add 100 tree instances
			auto tree_instance = std::make_unique<Tree>();
			for (int i = 0; i < 3; i++) { //generate and add 5 apple instances to the single tree
				auto apple = std::make_unique<Apple>();
				tree_instance->children.push_back(std::move(apple));
			}
			tree_instance->modelMatrix = generateRandomTreeModelMatrix();
			scene.objects.push_back(std::move(tree_instance));
		}


		auto tree_of_life = std::make_unique<AppleTree>();
		for (int i = 0; i < 20; i++) { //generate and add 5 apple instances to the single tree
			auto golden_apple = std::make_unique<GoldenApple>();
			tree_of_life->children.push_back(std::move(golden_apple));
		}
		scene.objects.push_back(std::move(tree_of_life));

		//fire
		//new comment to push
		auto fire = std::make_unique<Fire>();
		//auto smoke = std::make_unique<Smoke>();
		for (int i = 0; i < 50; i++) {
			auto particles = std::make_unique<Particles>();
			fire->children.push_back(std::move(particles));
		}
		//fire->children.push_back(std::move(smoke));
		scene.objects.push_back(std::move(fire));


		//create pig
		auto pig = std::make_unique<Pig>();
		auto gas1 = std::make_unique<Gas>();
		auto horseFly = std::make_unique<HorseFly>();
		gas1->children.push_back(std::move(horseFly));
		pig->children.push_back(std::move(gas1));
		scene.objects.push_back(std::move(pig));

		// Use basic texture shader (no lighting)
		auto shader = std::make_unique<ppgso::Shader>(texture_vert_glsl, texture_frag_glsl);
		scene.shader = std::move(shader);
	}

	void initCommon()
	{

		// moonlight

		// ambient

		// third light

		// player

		// backgrounds

	}

	void scene1_init() // scene 1
	{
		initBase();

		initCommon();

     	//...
	}

	void scene2_init()
	{
		scene.objects.clear();

		initCommon();


        //...
	}

public:

	ProjectWindow(int size) : Window{"project", size, size}
	{
		buffer_init();
		scene1_init();
	}

	void buffer_init()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		quadTexture.bind();

		unsigned int framebufferTexture;
		glGenTextures(1, &framebufferTexture);
		glBindTexture(GL_TEXTURE_2D, framebufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

		// Initialize framebuffer, its color texture (the sphere will be rendered to it) and its render buffer for depth info storage
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Set up render buffer that has a depth buffer and stencil buffer
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		// Associate the quadTexture with it
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, quadTexture.image.width, quadTexture.image.height);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, quadTexture.getTexture(), 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("Cannot create framebuffer!");
	}

	void buffer_set()
	{
		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glClearColor(0.0f, 0.0f, 0.0f, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void buffer_show()
	{
		resetViewport();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear the framebuffer
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto quadViewMatrix = glm::mat4{1.0f};
		quadViewMatrix = glm::lookAt(glm::vec3{0.0f, 0.0f, -1.0f}, scene.camera->help - glm::vec3{0.0f, 1.0f, -1.0f}, {0.0f, -1.0f, 0.0f});

		// Animate rotation of the quad
		auto quadModelMatrix = glm::mat4{1.0f};

		// Set shader inputs
		quadShader.use();
		quadShader.setUniform("ProjectionMatrix", scene.camera->perspective);
		quadShader.setUniform("ViewMatrix", quadViewMatrix);
		quadShader.setUniform("ModelMatrix", quadModelMatrix);
		quadShader.setUniform("Texture", quadTexture);
		quadMesh.render();
	}

	void onIdle()
	{
		buffer_set();

		// Track time
		static auto time = (float)glfwGetTime();
		float dTime = (float)glfwGetTime() - time;
		time = (float)glfwGetTime();

		// update
		scene.update(dTime);

		scene.render();
		buffer_show();
	}

	void onKey(int key, int scanCode, int action, int mods) override {
		if (action == GLFW_PRESS || action == GLFW_REPEAT)  // Handle both press and hold
		{
			//halo pome
			switch (scanCode) {
				case 124: // Right arrow
					scene.camera->turn_right = true;
				break;

				case 123: // Left arrow
					scene.camera->turn_left = true;
				break;

				case 126: // Up arrow (Go player)
					scene.camera->go_player = true;
				break;
			}
		}

		if (action == GLFW_RELEASE)  // Handle key release
		{
			switch (scanCode) {
				case 124: // Right arrow release
					scene.camera->turn_right = false;
				break;

				case 123: // Left arrow release
					scene.camera->turn_left = false;
				break;

				case 126: // Up arrow release
					scene.camera->go_player = false;
				break;
			}
		}
	}


};
