CFLAGS := -Wall -lm -lGL -lGLU -lglut -DGL_GLEXT_PROTOTYPES

all: draw

draw: src/main.cpp Makefile
		g++ $(CFLAGS) -o $@ $(filter %.cpp,$^)
