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
#include <shaders/phong_frag_glsl.h>
#include <shaders/phong_vert_glsl.h>
#include <shaders/point_frag_glsl.h>
#include <shaders/vertical_blur_frag_glsl.h>
#include <shaders/horizontal_blur_frag_glsl.h>


//my files
#include "objects/bee.h"
#include "scene.cpp"
#include "objects/ground.h"
#include "objects/tree.h"
#include "generator.h"
#include "objects/apple.h"
#include "objects/gas.h"
#include "objects/horseFly.h"
#include "src/objects/pig.h"
#include "objects/AppleTree.h"
#include "objects/fire.h"
#include "objects/goldenApple.h"
#include "objects/particles.h"
#include "objects/stone.h"
#include "objects/wolf.h"
#include "objects/grass.cpp"
#include "objects/lighthouse.cpp"
#include "objects/MySphere.h"
#include "generator.h"
#include "objects/crystal.h"

class ProjectWindow : public ppgso::Window
{
private:
	Scene scene;

	// Objects to render the framebuffer on to
	ppgso::Shader quadShader = {texture_vert_glsl, texture_frag_glsl};
	ppgso::Mesh quadMesh = {"quad.obj"};
	ppgso::Texture quadTexture = {1024, 1024};
	// ppenGL object ids for framebuffer and render buffer
	GLuint fbo = 1;
	GLuint rbo = 1;


	ppgso::Shader horizontalBlurShader;
	ppgso::Shader verticalBlurShader;
	GLuint blurFbo1, blurFbo2;
	GLuint blurTexture1, blurTexture2;


	glm::vec3 initControlPoints[4][4]{
		{ {-0.1, 0.1, 0}, {-0.05, 0.1, 0}, {0.05, 0.1, 0}, {0.1, 0.1, 0} },
		{ {-0.1, 0.05, 0}, {-0.05, 0.05, 0}, {0.05, 0.05, 0}, {0.1, 0.05, 0} },
		{ {-0.1, -0.05, 0}, {-0.05, -0.05, 0}, {0.05, -0.05, 0}, {0.1, -0.05, 0} },
		{ {-0.1, -0.1, 0}, {-0.05, -0.1, 0}, {0.05, -0.1, 0}, {0.1, -0.1, 0} }
	};

	bool applyBlur = false;

	void initBase() {
		// Camera setup
		auto camera = std::make_unique<Camera>(100.0f, (float)width / (float)height, 0.1f, 100.0f);
		scene.camera = std::move(camera);




		//scene graph

		//ground
		auto ground = std::make_unique<Ground>();


		//trees wit apples
		auto tree = std::make_unique<Tree>();
		for (int i = 0 ; i < 100; i++) {
			auto tree_instance = std::make_unique<Tree>();
			for (int i = 0; i < 3; i++) {
				auto apple = std::make_unique<Apple>();
				tree_instance->children.push_back(std::move(apple));
			}
			std::pair<glm::mat4, glm::mat4> tree_matrices = generateRandomTreeModelMatrix();
			tree_instance->modelMatrix = tree_matrices.first;
			tree_instance->childModelMatrix = tree_matrices.second;
			ground->children.push_back(std::move(tree_instance));
		}


		//spacial tree - apple tree with apples and and a stone
		 auto tree_of_life = std::make_unique<AppleTree>();
		 for (int i = 0; i < 40; i++) { //generate and add 5 apple instances to the single tree
		 	auto golden_apple = std::make_unique<GoldenApple>();
		 	tree_of_life->children.push_back(std::move(golden_apple));
		 }
		auto stone1 = std::make_unique<Stone>();
		tree_of_life->children.push_back(std::move(stone1));
		ground->children.push_back(std::move(tree_of_life));


		//fire with smoke - particles
		auto fire = std::make_unique<Fire>();
		for (int i = 0; i < 50; i++) {
			auto particles = std::make_unique<Particles>();
			fire->children.push_back(std::move(particles));
		}
		ground->children.push_back(std::move(fire));


		//stone structure with crystal
		auto crystal_stone = std::make_unique<Stone>();
		auto levitating_stone = std::make_unique<Stone>();
		auto back_stone = std::make_unique<Stone>();
		crystal_stone->position = glm::vec3(-3,0,-3);
		levitating_stone->position = glm::vec3(0,2,-0.15);
		back_stone->position = glm::vec3(0,0,-1.2);
		back_stone->scale = glm::vec3(0.07,0.2,0.07);
		auto crystal = std::make_unique<Crystal>();
		crystal_stone->children.push_back(std::move(crystal));
		crystal_stone->children.push_back(std::move(levitating_stone));
		crystal_stone->children.push_back(std::move(back_stone));
		ground->children.push_back(std::move(crystal_stone));


		//grass
		ground->children.push_back(std::make_unique<GrassPatch>(initControlPoints));

		//lighthouse
		ground->children.push_back(std::move(std::make_unique<Lighthouse>()));


		scene.objects.push_back(std::move(ground));

		// moon
		auto sphere = std::make_unique<MySphere>();
		scene.objects.push_back(std::move(sphere));

		// bee
		scene.objects.push_back(std::make_unique<Bee>());


		// pigs and wolves
		for (int i = 0; i < 8; ++i) {
			auto pig = std::make_unique<Pig>();
			if (i < 3) {
				scene.objects.push_back(std::make_unique<Wolf>());
				auto gas = std::make_unique<Gas>();
				auto horseFly = std::make_unique<HorseFly>();
				gas->children.push_back(std::move(horseFly));
				pig->children.push_back(std::move(gas));
			}
			scene.objects.push_back(std::move(pig));
		}


		auto shader = std::make_unique<ppgso::Shader>(phong_vert_glsl, phong_frag_glsl);
		auto colorShader = std::make_unique<ppgso::Shader>(phong_vert_glsl, point_frag_glsl);
		scene.shader = std::move(shader);
		scene.colorShader = std::move(colorShader);
	}


	void scene1_init() // scene 1
	{
		initBase();
	}

public:

	ProjectWindow(int size)
	: Window{"project", size, size},
	  horizontalBlurShader{texture_vert_glsl, horizontal_blur_frag_glsl},
	  verticalBlurShader{texture_vert_glsl, vertical_blur_frag_glsl}
	{
		buffer_init();
		scene1_init();
	}

	void buffer_init()
	{
	    glEnable(GL_DEPTH_TEST);
	    glDepthFunc(GL_LEQUAL);

	    glGenFramebuffers(1, &fbo);
	    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	    glBindTexture(GL_TEXTURE_2D, quadTexture.getTexture());
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, quadTexture.image.width, quadTexture.image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, quadTexture.getTexture(), 0);

	    glGenRenderbuffers(1, &rbo);
	    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, quadTexture.image.width, quadTexture.image.height);
	    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	        throw std::runtime_error("Cannot create framebuffer!");

	    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	    glGenFramebuffers(1, &blurFbo1);
	    glBindFramebuffer(GL_FRAMEBUFFER, blurFbo1);

	    glGenTextures(1, &blurTexture1);
	    glBindTexture(GL_TEXTURE_2D, blurTexture1);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, quadTexture.image.width, quadTexture.image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture1, 0);

	    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	        throw std::runtime_error("Cannot create blur framebuffer 1!");

	    glGenFramebuffers(1, &blurFbo2);
	    glBindFramebuffer(GL_FRAMEBUFFER, blurFbo2);

	    glGenTextures(1, &blurTexture2);
	    glBindTexture(GL_TEXTURE_2D, blurTexture2);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, quadTexture.image.width, quadTexture.image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture2, 0);

	    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	        throw std::runtime_error("Cannot create blur framebuffer 2!");

	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto quadViewMatrix = glm::mat4{1.0f};
		quadViewMatrix = glm::lookAt(glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, -1.0f});

		auto quadModelMatrix = glm::mat4{1.0f};

		quadShader.use();
		quadShader.setUniform("ProjectionMatrix", scene.camera->perspective);
		quadShader.setUniform("ViewMatrix", quadViewMatrix);
		quadShader.setUniform("ModelMatrix", quadModelMatrix);
		quadShader.setUniform("Texture", quadTexture);
		quadMesh.render();
	}

	//blur on key press "b"
	void onKey(int key, int scanCode, int action, int mods) override {
		if (key == GLFW_KEY_B && action == GLFW_PRESS) {
			applyBlur = !applyBlur;
		}
	}


	void onIdle()
	{
	    static auto time = (float)glfwGetTime();
	    float dTime = (float)glfwGetTime() - time;
	    time = (float)glfwGetTime();

	    glViewport(0, 0, quadTexture.image.width, quadTexture.image.height);
	    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	    glClearColor(0.0f, 0.0f, 0.0f, 0);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    scene.update(dTime);
	    scene.render();

	    if (applyBlur)
	    {
	        glBindFramebuffer(GL_FRAMEBUFFER, blurFbo1);
	        glClear(GL_COLOR_BUFFER_BIT);

	        horizontalBlurShader.use();
	        horizontalBlurShader.setUniform("image", 0);

	        float weight[5] = {0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f};
	        for (int i = 0; i < 5; ++i)
	        {
	            horizontalBlurShader.setUniform(("weight[" + std::to_string(i) + "]").c_str(), weight[i]);
	        }

	        horizontalBlurShader.setUniform("ProjectionMatrix", glm::mat4(1.0f));
	        horizontalBlurShader.setUniform("ViewMatrix", glm::mat4(1.0f));
	        horizontalBlurShader.setUniform("ModelMatrix", glm::mat4(1.0f));

	        glActiveTexture(GL_TEXTURE0);
	        glBindTexture(GL_TEXTURE_2D, quadTexture.getTexture());

	        quadMesh.render();

	        glBindFramebuffer(GL_FRAMEBUFFER, blurFbo2);
	        glClear(GL_COLOR_BUFFER_BIT);

	        verticalBlurShader.use();
	        verticalBlurShader.setUniform("image", 0);

	        for (int i = 0; i < 5; ++i)
	        {
	            verticalBlurShader.setUniform(("weight[" + std::to_string(i) + "]").c_str(), weight[i]);
	        }

	        verticalBlurShader.setUniform("ProjectionMatrix", glm::mat4(1.0f));
	        verticalBlurShader.setUniform("ViewMatrix", glm::mat4(1.0f));
	        verticalBlurShader.setUniform("ModelMatrix", glm::mat4(1.0f));

	        glActiveTexture(GL_TEXTURE0);
	        glBindTexture(GL_TEXTURE_2D, blurTexture1);

	        quadMesh.render();
	    }

	    resetViewport();
	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    quadShader.use();
	    quadShader.setUniform("ProjectionMatrix", glm::mat4(1.0f));
	    quadShader.setUniform("ViewMatrix", glm::mat4(1.0f));
	    quadShader.setUniform("ModelMatrix", glm::mat4(1.0f));
	    quadShader.setUniform("Texture", 0);

	    glActiveTexture(GL_TEXTURE0);
	    if (applyBlur)
	    {
	        glBindTexture(GL_TEXTURE_2D, blurTexture2); // Render blurred texture
	    }
	    else
	    {
	        glBindTexture(GL_TEXTURE_2D, quadTexture.getTexture()); // Render original scene texture
	    }

	    quadMesh.render();
	}
};
