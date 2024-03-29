#pragma region Include Statements
#define _USE_MATH_DEFINES
#define GLEW_STATIC

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <math.h> 
#include <filesystem>

#include <GL/glew.h>
// #include <GL/gl.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header

#include <glm/glm.hpp> // GL Math library header

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

// #include <freetype2/include/ft2build.h>
// #include FT_FREETYPE_H

#include "typedefs.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Light.h"
#include "Camera.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "utils.h"
#include "constTypes.h"


#pragma endregion
#pragma region String Literals

std::string vertexShaderSource = "/shaders/lit.vert";
std::string fragmentShaderSource = "/shaders/lit.frag";
std::string objPath = "/objs/";

#pragma endregion
#pragma region Variables
// Window dimensions
GLuint WIDTH = 1280, HEIGHT = 720;
Vector3 backgroundColor = Vector3(0.1f, 0.1f, 0.2f);

// Camera
Camera* mainCamera;

// Lights
std::vector<Light> lights;

// Game objects (all necessary components for rendering is in here)
std::vector<GameObject> gameObjects;

// Window
GLFWwindow* window;

MeshRenderer* meshRenderer;

// Inputs
Vector2 cameraMovement = Vector2(0.0f, 0.0f);

std::string windowTitle = "CENG469 HW1 OPENGL";

#pragma endregion

#pragma region Function Declarations
void CreateWindow();
void reshape(GLFWwindow* window, int w, int h);
void inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mainLoop(GLFWwindow* window);
void init();
void drawObjects();
void updateObjects();

#pragma endregion

void init()
{
	// Print all obj files in the directory
	std::string cwd = std::filesystem::current_path().string();
	objPath = cwd + objPath;
	GameObject gameObject = CreateGameObject("cube", objPath + "cube.obj", cwd + vertexShaderSource, cwd + fragmentShaderSource);
	gameObject.position = Vector3(0.0f, 0.0f, 0.0f);
	gameObject.scale = Vector3(1.0f, 1.0f, 1.0f);
	gameObject.rotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	gameObjects.push_back(gameObject);

	// Create camera
	mainCamera = new Camera();
	mainCamera->setPosition(Vector3(10.0f, 10.0f, 10.0f));
	mainCamera->setFieldOfView(60.0f);
	mainCamera->setAspectRatio((float)WIDTH / (float)HEIGHT);
	mainCamera->setNearPlane(0.1f);
	mainCamera->setFarPlane(100.0f);
	mainCamera->setType(PERSPECTIVE);
	// Turn camera to look at the cube
	mainCamera->setRotation(utilsLookAt(mainCamera->getPosition(), gameObject.position, mainCamera->getUp()));

	// Create a light with maximum intensity
	Light light1;
	light1.position = Vector3(0.0f, 0.0f, 3.0f);
	light1.ambient = Vector3(0.2f, 0.2f, 0.2f);
	light1.diffuse = Vector3(0.5f, 0.5f, 0.5f);
	light1.specular = Vector3(1.0f, 1.0f, 1.0f);
	light1.constant = 1.0f;
	light1.linear = 0.09f;
	light1.quadratic = 0.032f;
	
	lights.push_back(light1);
	
	// Create mesh renderer
	meshRenderer = new MeshRenderer();
	meshRenderer->SetLights(lights);
	meshRenderer->SetCamera(mainCamera);
}
void updateObjects()
{
	// handle camera movement
	if(cameraMovement.x != 0.0f || cameraMovement.y != 0.0f)
	{
		Vector3 cameraForward = mainCamera->getForward();
		Vector3 cameraRight = mainCamera->getRight();
		Vector3 cameraUp = mainCamera->getUp();
		Vector3 cameraPosition = mainCamera->getPosition();
		float cameraSpeed = 0.1f;
		cameraPosition += cameraForward * cameraMovement.y * cameraSpeed;
		cameraPosition += cameraRight * cameraMovement.x * cameraSpeed;
		mainCamera->setPosition(cameraPosition);
		// std::cout << "Camera Position: ";
		// printVector3Formatted(cameraPosition);
	}
}
void drawObjects()
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		GameObject gameObject = gameObjects[i];

		meshRenderer->Draw(gameObject);
		assert(glGetError() == GL_NO_ERROR);
	}
}

int main(int argc, char** argv)
{
	CreateWindow();
	return 0;
}
void reshape(GLFWwindow* window, int w, int h)
{
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;

	WIDTH = w;
	HEIGHT = h;

	glViewport(0, 0, w, h);
}
void inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);

		if (key == GLFW_KEY_W) cameraMovement.y += 1.0f;
		if (key == GLFW_KEY_S) cameraMovement.y += -1.0f;
		if (key == GLFW_KEY_A) cameraMovement.x += -1.0f;
		if (key == GLFW_KEY_D) cameraMovement.x += 1.0f;

		if (key == GLFW_KEY_U) backgroundColor.x += 0.1f;
		if (key == GLFW_KEY_J) backgroundColor.x -= 0.1f;
		if (key == GLFW_KEY_I) backgroundColor.y += 0.1f;
		if (key == GLFW_KEY_K) backgroundColor.y -= 0.1f;
		if (key == GLFW_KEY_O) backgroundColor.z += 0.1f;
		if (key == GLFW_KEY_L) backgroundColor.z -= 0.1f;
	}
	else if(action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W) cameraMovement.y -= 1.0f;
		if (key == GLFW_KEY_S) cameraMovement.y -= -1.0f;
		if (key == GLFW_KEY_A) cameraMovement.x -= -1.0f;
		if (key == GLFW_KEY_D) cameraMovement.x -= 1.0f;
	}

}
void mainLoop(GLFWwindow* window)
{
	while (!glfwWindowShouldClose(window))
	{
		// Clear the colorbuffer
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Movement
		updateObjects();

		// Print avg fps
		static double previousSeconds = glfwGetTime();
		static int frameCount;
		double elapsedSeconds = glfwGetTime() - previousSeconds;
		if (elapsedSeconds > 0.25)
		{
			previousSeconds = glfwGetTime();
			double fps = (double)frameCount / elapsedSeconds;
			double msPerFrame = 1000.0 / fps;

			std::ostringstream outs;
			outs.precision(3); // decimal places
			outs << std::fixed
				<< "FPS: " << fps << " Frame Time: " << msPerFrame << "(ms)";
			// Append fps to window title
			glfwSetWindowTitle(window, (windowTitle + " - " + outs.str()).c_str());

			frameCount = 0;
		}
		frameCount++;

		// Draw game objects
		drawObjects();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *msg, const void *data)
{
    std::string source_;
    std::string type_;
    std::string severity_;

    switch (source)
	{
		case GL_DEBUG_SOURCE_API:             source_ = "API";             break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_ = "WINDOW_SYSTEM";   break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: source_ = "SHADER_COMPILER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     source_ = "THIRD_PARTY";     break;
		case GL_DEBUG_SOURCE_APPLICATION:     source_ = "APPLICATION";     break;
		case GL_DEBUG_SOURCE_OTHER:           source_ = "OTHER";           break;
		default:                              source_ = "<SOURCE>";        break;
	}

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               type_ = "ERROR";               break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_ = "DEPRECATED_BEHAVIOR"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_ = "UDEFINED_BEHAVIOR";   break;
		case GL_DEBUG_TYPE_PORTABILITY:         type_ = "PORTABILITY";         break;
		case GL_DEBUG_TYPE_PERFORMANCE:         type_ = "PERFORMANCE";         break;
		case GL_DEBUG_TYPE_OTHER:               type_ = "OTHER";               break;
		case GL_DEBUG_TYPE_MARKER:              type_ = "MARKER";              break;
		default:                                type_ = "<TYPE>";              break;
	}

	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         severity_ = "HIGH";         break;
		case GL_DEBUG_SEVERITY_MEDIUM:       severity_ = "MEDIUM";       break;
		case GL_DEBUG_SEVERITY_LOW:          severity_ = "LOW";          break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: severity_ = "NOTIFICATION"; break;
		default:                             severity_ = "<SEVERITY>";   break;
	}


	std::cout << id << ": " << type_ << " of " << severity_ << " severity, raised from " << source_ << ": " << msg << std::endl;
}

void EnableGLDebugging()
{
	glDebugMessageCallback(GLDebugMessageCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
void CreateWindow()
{
	if(!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // uncomment this if you want to debug
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this if on MacOS
	
	glEnable(GL_DEPTH_TEST);
	window = glfwCreateWindow(WIDTH, HEIGHT, "CENG469 HW1 OPENGL", NULL, NULL);

	if(!window)
	{
		glfwTerminate();
		std::cout << "Failed to create window" << std::endl;
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	std::cout << "OpenGL version supported by this platform " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
	std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

	// Initialize GLEW to setup the OpenGL Function pointers
	int initState = glewInit();
	if (initState != GLEW_OK)
	{
		std::cerr << "Error initializing GLEW: " << glewGetErrorString(initState) << ". Enum:" << initState << std::endl;
		exit(EXIT_FAILURE);
	}

	char rendererInfo[512] = { 0 };

	strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER));
	strcat(rendererInfo, " - ");
	strcat(rendererInfo, (const char*)glGetString(GL_VERSION));

	// Append renderer info to window title
	windowTitle += " - ";
	windowTitle += rendererInfo;
	
	glfwSetWindowTitle(window, windowTitle.c_str());
	glfwSetKeyCallback(window, inputCallback);
	glfwSetWindowSizeCallback(window, reshape);
	
	EnableGLDebugging();

	init();

	reshape(window, WIDTH, HEIGHT); // need to call this once ourselves
	mainLoop(window); // this does not return unless the window is closed

	glfwDestroyWindow(window);
	glfwTerminate();
}
