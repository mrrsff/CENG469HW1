#pragma region Include Statements

#define _USE_MATH_DEFINES
#define GLEW_STATIC
#define CALLBACK
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

#include "typedefs.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Light.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "ShaderProgram.h"
#include "utils.h"
#include "constTypes.h"
#include "printExtensions.h"
#include "extensions/MeshSubdivider.h"


#pragma endregion

#pragma region String Literals

std::string vertexShaderSource = "./shaders/lit.vert";
std::string fragmentShaderSource = "./shaders/lit.frag";
std::string geometryShaderSource = "./shaders/lit.geom";
std::string objPath = "/objs/";

#pragma endregion
#pragma region Variables

enum DrawMode
{
	SOLID = 0,
	LINE = 1,
	WIREFRAME = 2
};
DrawMode drawMode = SOLID;

// Window dimensions
GLuint WIDTH = 1280, HEIGHT = 720;
Vector3 backgroundColor = Vector3(0.2f, 0.2f, 0.4f);
std::string windowTitle = "CENG469 HW1 OPENGL";

// Window
GLFWwindow* window;

// Scene components
Camera* mainCamera;
std::vector<Light> lights;
MeshRenderer* meshRenderer;

// Game objects (all necessary components for rendering is in here)
std::vector<GameObject*> gameObjects;
int subdivisionLevel = 0;
std::map<GameObject*, std::vector<Mesh*>> subdividedMeshes;
std::vector<Material> materials;
bool stopGameObjects = false;
bool testMode = false;
Vector2 offsets = Vector2(-0.5f, -0.6f);
Vector3 objScale = Vector3(1.0f, 1.0f, 1.0f);

Mesh* triangleCubeMesh;
Mesh* quadCubeMesh;
int subdivLevel = 10;

double context_time = 0.0;

Vector2 cameraMovement = Vector2(0.0f, 0.0f);

#pragma endregion

#pragma region Function Declarations
void CreateWindow();
void reshape(GLFWwindow* window, int w, int h);
void inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mainLoop(GLFWwindow* window);
void init();
void drawObjects();
void updateObjects();
void nextDrawMode();
#pragma endregion

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_LINE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Print all obj files in the directory
	std::string cwd = std::filesystem::current_path().string();
	objPath = cwd + objPath;

	int objCount = 4;
	
	// Create materials for each object (each object must have a different color)
	for (int i = 0; i < objCount; i++)
	{
		double red = (double)(i % 3);
		double green = (double)((i + 1) % 3);
		double blue = (double)((i + 2) % 3);
		double randomOffset = (double)(rand() % 100) / 100.0f - 0.5f;
		Material material = Material();

		material.ambient = Vector3(red + randomOffset, green + randomOffset, blue + randomOffset);
		material.diffuse = Vector3(red + randomOffset, green + randomOffset, blue + randomOffset);

		material.specular = Vector3(1.0f, 1.0f, 1.0f);
		material.shininess = 128.0f;
		materials.push_back(material);		
	}

	// Load obj files
	triangleCubeMesh = ParseObjFile((objPath + "cube.obj").c_str(), true, true);
	quadCubeMesh = ParseObjFile((objPath + "cube.obj").c_str(), false, true);
	
	// Create gameobjects and cache subdivided meshes
	for (int i = 0; i < objCount; i++)
	{
		ShaderProgram* shader = new ShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
		assert(shader != nullptr);

		GameObject* gameObject = new GameObject(shader);
		gameObject->SetShader(shader);
		gameObject->name = "Cube " + std::to_string(i);
		Mesh* mesh = i % 2 == 0 ? triangleCubeMesh : quadCubeMesh;
		// Print mesh triangles
		gameObject->SetMesh(mesh);
		gameObject->SetScale(objScale);
		gameObject->SetMaterial(&materials[i]);
		gameObject->SetPosition(Vector3(5.0f * i, 0.0f, 0.0f));
		gameObjects.push_back(gameObject);

		// Cache subdivided meshes
		subdividedMeshes[gameObject].push_back(mesh);
		for (int j = 1; j < subdivLevel; j++)
		{
			Mesh *subdividedMesh = MeshSubdivider::subdivideCatmullClark(subdividedMeshes[gameObject][j - 1], 1);
			if (subdividedMesh == nullptr)
			{
				std::cout << "Subdivision failed" << std::endl;
				exit(-1);
			}
			subdividedMeshes[gameObject].push_back(subdividedMesh);
		}
	}

	Vector3 avgObjectPosition = Vector3(0.0f, 0.0f, 0.0f);
	for (GameObject* obj: gameObjects)
	{
		avgObjectPosition += obj->position;
	}
	avgObjectPosition /= gameObjects.size();

	float zDistance = gameObjects.size() * 3.0f;
	Vector3 cameraPos = avgObjectPosition;
	cameraPos.z += zDistance;

	// Create camera
	mainCamera = new Camera();
	mainCamera->setPosition(cameraPos);
	mainCamera->setFieldOfView(60.0f);
	mainCamera->setAspectRatio((float)WIDTH / (float)HEIGHT);
	mainCamera->setNearPlane(0.1f);
	mainCamera->setFarPlane(1000.0f);
	mainCamera->setType(PERSPECTIVE);
	// Turn camera to look at the cube
	mainCamera->setRotation(utilsLookAt(mainCamera->getPosition(), avgObjectPosition, mainCamera->getUp()));

	// Create a very powerful light on top of the camera
	Light light;
	light.position = mainCamera->getPosition() + Vector3(0.0f, 5.0f, 0.0f);
	light.ambient = Vector3(1,1,1);
	light.diffuse = Vector3(1,1,1);
	light.specular = Vector3(1.0f, 1.0f, 1.0f);
	light.constant = 0.8f;
	light.linear = 0;
	light.quadratic = 0;
	light.colorIntensity = Vector3(1.0f, 1.0f, 1.0f);
	light.colorIntensity *= 0.2f;
	lights.push_back(light);

	// Create mesh renderer
	meshRenderer = new MeshRenderer();
	meshRenderer->SetLights(lights);
	meshRenderer->SetCamera(mainCamera);
}
void updateObjects()
{
	if(stopGameObjects) return;

	// handle camera movement
	if(testMode && (cameraMovement.x != 0.0f || cameraMovement.y != 0.0f))
	{
		Vector3 cameraForward = mainCamera->getForward();
		Vector3 cameraRight = mainCamera->getRight();
		Vector3 cameraUp = mainCamera->getUp();
		Vector3 cameraPosition = mainCamera->getPosition();
		float cameraSpeed = 0.1f;
		if(mainCamera->getType() == ORTHOGRAPHIC)
		{
			cameraPosition += cameraUp * cameraMovement.y * cameraSpeed;
		}
		else cameraPosition += cameraForward * cameraMovement.y * cameraSpeed;
		cameraPosition += cameraRight * cameraMovement.x * cameraSpeed;
		mainCamera->setPosition(cameraPosition);

		meshRenderer->UpdateCameraUBO();
	}

	/* Update game objects with these rules:
		If i%3==0, rotate around y-axis
		If i%3==1, move up and down (sin wave)
		If i%3==2, scale up and down (sin wave)
	*/
	float sinValue = sin(context_time) * 0.5f;
	float skewedSinValue = sin((context_time + 1.0f) / 4) * 0.5f;

	for (int i = 0; i < gameObjects.size(); i++)
	{
		GameObject* gameObject = gameObjects[i];
		if(i % 4 == 0)
		{
			Quaternion rotation = gameObject->GetRotation();
			Quaternion rotationDelta = utilsFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 0.5f);
			rotation = rotation * rotationDelta;
			gameObject->SetRotation(rotation);
		}
		else if(i % 4 == 1)
		{
			Vector3 position = gameObject->GetPosition();
			position.y = sin(glfwGetTime() + i * 2.0f) * 2.0f;
			gameObject->SetPosition(position);
		}
		else if(i % 4 == 2)
		{
			Vector3 scale = gameObject->GetScale();
			scale.x = 1.0f + skewedSinValue * 0.2f;
			scale.y = 1.0f + skewedSinValue * 0.2f;
			scale.z = 1.0f + skewedSinValue * 0.2f;
			gameObject->SetScale(scale);
		}
		else if(i % 4 == 3)
		{
			// Rotate around the game objects
			Vector3 position = gameObject->GetPosition();
			Vector3 center = Vector3(0.0f, 0.0f, 0.0f);
			for (int j = 0; j < gameObjects.size(); j++)
			{
				center += gameObjects[j]->GetPosition();
			}
			center /= gameObjects.size();
			Vector3 cameraForward = mainCamera->getForward();
			Vector3 cameraRight = mainCamera->getRight();
			float radius = 7.0f;
			float angle = context_time / 10;
			position.x = center.x + cameraRight.x * radius * sin(angle) + cameraForward.x * radius * cos(angle);
			position.z = center.z + cameraRight.z * radius * sin(angle) + cameraForward.z * radius * cos(angle);
			gameObject->SetPosition(position);
		}
	}

}
void increaseSubdivisionLevel()
{
	// Increase subdivision level of rendered mesh
	int newSubdivisionLevel = subdivisionLevel < subdivLevel - 1 ? subdivisionLevel + 1 : subdivLevel - 1;
	if (newSubdivisionLevel == subdivisionLevel) return;
	subdivisionLevel = newSubdivisionLevel;
	for (GameObject* obj: gameObjects)
	{
		Mesh* mesh = subdividedMeshes[obj][subdivisionLevel];
		obj->SetMesh(mesh);
	}

	std::cout << "Subdivision Level: " << subdivisionLevel << std::endl;
}
void decreaseSubdivisionLevel()
{
	// Decrease subdivision level of the mesh
	int newSubdivisionLevel = subdivisionLevel > 0 ? subdivisionLevel - 1 : 0;
	if (newSubdivisionLevel == subdivisionLevel) return;
	subdivisionLevel = newSubdivisionLevel;
	for (GameObject* obj: gameObjects)
	{
		Mesh* mesh = subdividedMeshes[obj][subdivisionLevel];
		obj->SetMesh(mesh);
	}

	std::cout << "Subdivision Level: " << subdivisionLevel << std::endl;
}
void drawObjects()
{
	// If line mode, we should use 2 passes. One for updating depth buffer, one for drawing lines
	if (drawMode == LINE)
	{
		// This should not draw anything, only update depth buffer
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glPolygonOffset(0,0);
		// Draw game objects
		for (int i = 0; i < gameObjects.size(); i++)
		{
			GameObject* gameObject = gameObjects[i];
			// Draw game object
			meshRenderer->Draw(gameObject);
			assert(glGetError() == GL_NO_ERROR);
		}
		// Draw game objects as lines
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glPolygonOffset(offsets.x, offsets.y);
	}
	for (int i = 0; i < gameObjects.size(); i++)
	{
		GameObject* gameObject = gameObjects[i];
		// Draw game object
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
	mainCamera->setAspectRatio((float)w / (float)h);

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

		// if (key == GLFW_KEY_U) backgroundColor.x += 0.1f;
		// if (key == GLFW_KEY_J) backgroundColor.x -= 0.1f;
		// if (key == GLFW_KEY_I) backgroundColor.y += 0.1f;
		// if (key == GLFW_KEY_K) backgroundColor.y -= 0.1f;
		// if (key == GLFW_KEY_O) backgroundColor.z += 0.1f;
		// if (key == GLFW_KEY_L) backgroundColor.z -= 0.1f;

		// Stop game object movement
		if (key == GLFW_KEY_S) stopGameObjects = !stopGameObjects;

		// Open and close wireframe mode (no culling)
		if (key == GLFW_KEY_M) nextDrawMode();

		// Increase subdivision level
		if (key == GLFW_KEY_T) increaseSubdivisionLevel();

		// Decrease subdivision level
		if (key == GLFW_KEY_E) decreaseSubdivisionLevel();

		if (key == GLFW_KEY_R) context_time = 0;

		// Change perspective
		if (key == GLFW_KEY_P)
		{
			if (mainCamera->getType() == ORTHOGRAPHIC)
			{
				mainCamera->setType(PERSPECTIVE);
				std::cout << "Camera Type: Perspective" << std::endl;
			}
			else
			{
				mainCamera->setType(ORTHOGRAPHIC);
				std::cout << "Camera Type: Orthographic" << std::endl;
			}
			meshRenderer->UpdateCameraUBO();
		}
	}
	else if(action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W) cameraMovement.y -= 1.0f;
		if (key == GLFW_KEY_S) cameraMovement.y -= -1.0f;
		if (key == GLFW_KEY_A) cameraMovement.x -= -1.0f;
		if (key == GLFW_KEY_D) cameraMovement.x -= 1.0f;
	}
	
}
void nextDrawMode()
{
	/* Draw Modes

		0: Solid
			- Fill
		1: Line
			- Culling
			- Line
			- With 2 passes one for updating depth buffer, one for drawing lines
		2: Wireframe
			- No Culling
			- Line
	*/
	drawMode = (DrawMode)((drawMode + 1) % 3);
	switch (drawMode)
	{
		case SOLID:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_CULL_FACE);
			std::cout << "Draw Mode: Solid" << std::endl;
			break;
		case WIREFRAME:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
			std::cout << "Draw Mode: Wireframe" << std::endl;
			break;
		case LINE:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_CULL_FACE);
			std::cout << "Draw Mode: Line" << std::endl;
			break;
	}
}

void mousePosInputCallback(GLFWwindow* window, double xpos, double ypos)
{
	return;
	// Move camera with mouse
	static bool rotateCamera = false;
	static double lastX = 0;
	static double lastY = 0;
	if (rotateCamera)
	{
		double xoffset = lastX - xpos; // Inverted x-axis
		double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		Quaternion rotation = mainCamera->getRotation();
		Quaternion yaw = utilsFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -xoffset);
		Quaternion pitch = utilsFromAxisAngle(mainCamera->getRight(), yoffset);
		Quaternion newRotation = yaw * pitch * rotation;

		mainCamera->setRotation(newRotation);

		meshRenderer->UpdateCameraUBO();
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		rotateCamera = true;
		lastX = xpos;
		lastY = ypos;		
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		rotateCamera = false;
	}
}
void mainLoop(GLFWwindow* window)
{
	while (!glfwWindowShouldClose(window))
	{
		// Clear the colorbuffer
		// glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		if(!stopGameObjects)
			context_time += elapsedSeconds;
		// Movement
		updateObjects();

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

	// char rendererInfo[512] = { 0 };

	// strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER));
	// strcat(rendererInfo, " - ");
	// strcat(rendererInfo, (const char*)glGetString(GL_VERSION));

	// // Append renderer info to window title
	// windowTitle += " - ";
	// windowTitle += rendererInfo;
	
	glfwSetWindowTitle(window, windowTitle.c_str());
	glfwSetKeyCallback(window, inputCallback);
	glfwSetCursorPosCallback(window, mousePosInputCallback);
	glfwSetWindowSizeCallback(window, reshape);
	// Set timeout for polling events
	glfwWaitEventsTimeout(0.01); // Every 10ms (not fps-limited)
	
	EnableGLDebugging();

	init();

	reshape(window, WIDTH, HEIGHT); // need to call this once ourselves
	mainLoop(window); // this does not return unless the window is closed

	glfwDestroyWindow(window);
	glfwTerminate();
}
