includes = -I./external/glm -I./external/glfw/include -I./external/glew/include -I./external/ -I./external/glm/ -I./external/freetype2/ -I./external/freetype2/include
links = -lglfw -lpthread -lGLEW -lGL
flags = -g
output = test

all:
	g++ src/main.cpp -o $(output) $(includes) $(links) $(flags)

clean:
	rm -f test

run:
	make all
	./test