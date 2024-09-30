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


class ProjectWindow : public ppgso::Window
{
private:
	Scene scene;
	int current_scene = 0;

	// Objects to render the framebuffer on to
	// ppgso::Shader quadShader = {convolution_vert_glsl, convolution_frag_glsl};
	ppgso::Shader quadShader = {texture_vert_glsl, texture_frag_glsl};
	ppgso::Mesh quadMesh = {"quad.obj"};
	ppgso::Texture quadTexture = {1024, 1024};

	// OpenGL object ids for framebuffer and render buffer
	GLuint fbo = 1;
	GLuint rbo = 1;

	void initBase()
	{
		// camera

		// shader and light

	}

	void initCommon()
	{
		scene.light_positions.clear();

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

	}

	void buffer_set()
	{

	}

	void buffer_show()
	{

	}

	void onIdle()
	{

	}

	void onKey(int key, int scanCode, int action, int mods) override
	{
		if (action == GLFW_PRESS)
		{
			switch (scanCode)
			{
			case 38:
			case 113:
				// left
				scene.move_left = true;
				break;
			case 40:
			case 114:
				// right
				scene.move_right = true;
				break;
			case 65:
				// spacebar
				if (!scene.jump)
					scene.jump = true;
				break;
			case 77:
				scene.camera->go_boundary_right = true;
				scene.camera->go_boundary_left = false;
				break;
			case 75:
				scene.camera->go_boundary_left = true;
				scene.camera->go_boundary_right = false;
				break;
			case 76:
				scene.camera->go_boundary_left = false;
				scene.camera->go_boundary_right = false;
				scene.camera->go_player = true;
				break;
			}
		}
		if (action == GLFW_RELEASE)
		{
			switch (scanCode)
			{
			case 38:
			case 113:
				// left
				scene.move_left = false;
				break;
			case 40:
			case 114:
				// right
				scene.move_right = false;
				break;
			}
		}
	}
};