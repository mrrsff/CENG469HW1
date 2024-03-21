#define _USE_MATH_DEFINES

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

#include <GL/glew.h>
//#include <OpenGL/gl3.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header

#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <freetype2/include/ft2build.h>
#include FT_FREETYPE_H

#include "typedefs.h"
#include "GameObject.h"

int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
	std::cout << "Hello, World!" << std::endl;
	return 0;
}