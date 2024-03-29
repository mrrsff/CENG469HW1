OSFLAG :=
ifeq ($(OS),Windows_NT)
	OSFLAG = -D WIN32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSFLAG = -D LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		OSFLAG = -D OSX
	endif
endif

ifeq ($(OSFLAG),-D WIN32) # Windows
	includes = -I./external/glm -I./external -I./external/glfw/include -I./external/glew -I./external/freetype -I./src
else
	includes = -I./external/** -I./external -I./src
endif

vendorsDir = ./external
glewLib = $(vendorsDir)/glew/lib
glfwLib = $(vendorsDir)/glfw/lib
freetypeLib = $(vendorsDir)/freetype/lib
glutLib = $(vendorsDir)/glut

links = -lpthread -lGL -L$(glfwLib) -lglfw -L$(glewLib) -lGLEW -lfreetype
flags = -DGL_SILENCE_DEPRECATION -DGLM_ENABLE_EXPERIMENTAL -O3
output = CENG469HW1
objectDir = ./core
objectFiles = $(objectDir)/*.o

all: 
	@echo "Building objects..."
	@make -s objs
	@echo "Linking..."
	@make -s main
	
main:
	@g++ main.cpp $(objectFiles) $(includes) $(links) $(flags) $(OSFLAG) -o $(output)
	@echo "Done!"

clean:
	@rm -f $(output)

objs:
	@g++ -c src/*.cpp $(includes) $(flags)
	@mv *.o $(objectDir)
run:
	@make -s all
	./$(output)