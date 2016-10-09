#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>      

#include <GL/glew.h>

#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main

#include "NFD/nfd.h"

#include "shaders.h"
#include "helperFunctions.h"

#include "myObject3D.h"

using namespace std;

// SDL variables
SDL_Window* window;
SDL_GLContext glContext;

int window_width = 640;
int window_height = 480;

int mouse_position[2];
bool mouse_button_pressed = false;
bool quit = false;

// Camera parameters.
glm::vec3 camera_eye = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camera_forward = glm::vec3(0.0f, 0.0f, -1.0f);

// Projection parameters.
float fovy = 45.0f;
float zNear = 0.2f;
float zFar = 4000;

// OpenGL shader variables
GLuint vertexshader, fragmentshader, shaderprogram1;

 // Mesh object
myObject3D *obj1;

// Process the event.  
void processEvents(SDL_Event current_event)
{
	switch (current_event.type)
	{
		// window close button is pressed
		case SDL_QUIT:
		{
			quit = true;
			break;
		}
		case SDL_KEYDOWN:
		{
			if (current_event.key.keysym.sym == SDLK_ESCAPE)
				quit = true;
			else if (current_event.key.keysym.sym == SDLK_UP)
				camera_eye += 0.1f * camera_forward;
			else if (current_event.key.keysym.sym == SDLK_DOWN)
				camera_eye -= 0.1f * camera_forward;
			else if (current_event.key.keysym.sym == SDLK_LEFT)
				rotate(camera_forward, camera_up, 0.05f, true);
			else if (current_event.key.keysym.sym == SDLK_RIGHT)
				rotate(camera_forward, camera_up, -0.05f, true);
			else if (current_event.key.keysym.sym == SDLK_o)
			{
				nfdchar_t *outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj", NULL, &outPath);
				if (result != NFD_OKAY) return;
				myObject3D *obj_tmp = new myObject3D();
				if (!obj_tmp->readMesh(outPath))
				{
					delete obj_tmp;
					return;
				}
				delete obj1;
				obj1 = obj_tmp;
				obj1->normalize();
				obj1->computeNormals();
				obj1->createObjectBuffers();
			}
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			mouse_position[0] = current_event.button.x;
			mouse_position[1] = window_height - current_event.button.y;
			mouse_button_pressed = true;
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			mouse_button_pressed = false;
			break;
		}
		case SDL_MOUSEMOTION:
		{
			int x = current_event.motion.x;
			int y = window_height - current_event.motion.y;

			int dx = x - mouse_position[0];
			int dy = y - mouse_position[1];

			mouse_position[0] = x;
			mouse_position[1] = y;

			if ( (dx == 0 && dy == 0) || !mouse_button_pressed ) break;

			float vx = (float)dx / (float)window_width;
			float vy = (float)dy / (float)window_height;
			float theta = 4.0f * (fabs(vx) + fabs(vy));

			glm::vec3 camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
			glm::vec3 tomovein_direction = -camera_right * vx + -camera_up * vy;

			if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
			{
				glm::vec3 rotation_axis = glm::normalize(glm::cross(tomovein_direction, camera_forward));

				rotate(camera_forward, rotation_axis, theta, true);
				rotate(camera_up, rotation_axis, theta, true);
				rotate(camera_eye, rotation_axis, theta, false);
			}
			else if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
			{
				camera_eye += 1.6f * tomovein_direction;
			}
			break;
		}
		case SDL_WINDOWEVENT:
		{
			if (current_event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				window_width = current_event.window.data1;
				window_height = current_event.window.data2;
			}
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			if (current_event.wheel.y < 0)
				camera_eye -= 0.1f * camera_forward;
			else if (current_event.wheel.y > 0)
				camera_eye += 0.1f * camera_forward;
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	// Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Initialize video subsystem
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
	// Create window
	window = SDL_CreateWindow("IT-5102E-16", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	// Create OpenGL context
	glContext = SDL_GL_CreateContext(window);

	// Initialize glew
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// Setting up OpenGL shaders
	vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl");
	fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl");
	shaderprogram1 = initprogram(vertexshader, fragmentshader);

	// Read up the scene
	obj1 = new myObject3D();
	if (!obj1->readMesh("hand.obj")) return 0;
	obj1->normalize();
	obj1->computeNormals();
	obj1->createObjectBuffers();
 
	// Game loop
	while (!quit)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, window_width, window_height);

		glm::mat4 projection_matrix = glm::perspective(glm::radians(fovy), (float) window_width / (float)window_height, zNear, zFar);
		glUniformMatrix4fv(glGetUniformLocation(shaderprogram1, "myprojection_matrix"), 
			               1, GL_FALSE, &projection_matrix[0][0]);

		glm::mat4 view_matrix = glm::lookAt(camera_eye, camera_eye + camera_forward, camera_up);
		glUniformMatrix4fv(glGetUniformLocation(shaderprogram1, "myview_matrix"), 
			               1, GL_FALSE, &view_matrix[0][0]);

		glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix)));
		glUniformMatrix3fv(glGetUniformLocation(shaderprogram1, "mynormal_matrix"), 
			               1, GL_FALSE, &normal_matrix[0][0]);

		obj1->displayObject(0);
		//obj1->displayNormals();

		SDL_GL_SwapWindow(window);

		SDL_Event current_event;
		while (SDL_PollEvent(&current_event) != 0)
			processEvents(current_event);
	}
	
	// Freeing resources before exiting.
	// Destroy window
	if (glContext) SDL_GL_DeleteContext(glContext);
	if (window) SDL_DestroyWindow(window);

	//Freeing up OpenGL resources.
	delete obj1;
	glDeleteProgram(shaderprogram1);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}