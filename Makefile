vendorsDir = ./external
glewLib = $(vendorsDir)/glew/lib
glfwLib = $(vendorsDir)/glfw/lib
freetypeLib = $(vendorsDir)/freetype/lib
glutLib = $(vendorsDir)/glut

includes = -I./external/** -I./external -I./src
links = -lGL -L$(glfwLib) -lglfw -L$(glewLib) -lGLEW -lfreetype
flags = -DGL_SILENCE_DEPRECATION -DGLM_ENABLE_EXPERIMENTAL -O3 -std=c++17 -w -Wfatal-errors -ggdb3 -pedantic -fsanitize=address
output = main
objectDir = ./core
objectFiles = $(objectDir)/*.o

extensionsDir = ./extensions
extensionsFiles = $(extensionsDir)/*.cpp
extensionsObjectFiles = $(objectDir)/*.o

debuggerPath = /mnt/d/WSL/renderdoc_1.31/bin/qrenderdoc
logName = MemRep.log

.PHONY: run clean $(output) debug gdb valgrind
 
all: $(output)

$(output):
	@echo "Building object files..."
	@make -s objects
	@echo "Linking object files to $(output)..."
	@echo "Building extensions..."
	@make -s extensions
	@echo "Linking extensions..."
	@g++ main.cpp $(objectFiles) $(includes) $(links) $(flags) -o $(output)
	@echo "Finished linking."

$(objectDir)/%.o: src/%.cpp
	@mkdir -p $(objectDir)
	@echo "Building $@..."
	@g++ -c $< $(includes) $(flags) -o $@

objects: $(patsubst src/%.cpp, $(objectDir)/%.o, $(wildcard src/*.cpp))
	@echo "Finished building object files."

$(objectDir)/%.o: $(extensionsDir)/%.cpp
	@mkdir -p $(objectDir)
	@echo "Building $@..."
	@g++ -c $< $(includes) $(flags) -o $@

extensions: $(patsubst $(extensionsDir)/%.cpp, $(objectDir)/%.o, $(wildcard $(extensionsDir)/*.cpp))
	@echo "Finished building extension object files."

clean:
	@rm -f $(output)
	@rm -f $(objectDir)/*.o
	@echo "Removed $(output) and object files."

run: 
	@make -s all
	@echo "Running $(output)..."
	@./$(output)

debug:
	@make -s all
	@echo "Running $(output) with debugger..."
	@$(debuggerPath)

gdb:
	@make -s all
	@echo "Running $(output) with gdb..."
	@gdb -ex run ./$(output)

valgrind:
	@make -s all
	@echo "Running $(output) with valgrind..."
	@valgrind --leak-check=full --track-origins=yes --log-file="$(logName)" ./$(output)