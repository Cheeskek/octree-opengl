CPP_COMPILER = g++
CPP_FLAGS = -std=c++20 -Wall -Wextra
CPP_DEBUG_FLAGS = -D_DEBUG=1
CPP_RELEASE_FLAGS = -O2
LDFLAGS = -lGL -lGLEW -lglfw -lX11 -lXrandr
CPP_SOURCE_FILES = src/headers/shader.cpp src/headers/inputHandler.cpp src/headers/engine.cpp src/main.cpp

all:
	$(CPP_COMPILER) $(CPP_FLAGS) $(CPP_RELEASE_FLAGS) $(LDFLAGS) $(CPP_SOURCE_FILES) -o build/main

debug:
	$(CPP_COMPILER) $(CPP_FLAGS) $(CPP_DEBUG_FLAGS) $(LDFLAGS) $(CPP_SOURCE_FILES) -o debug_build/main 

test: debug
	./debug_build/main 

.PHONY: test
