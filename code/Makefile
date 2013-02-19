CFLAGS := -g -Wall -lm -lGL -lGLEW -lglut

all: hand_renderer

hand_renderer: \
	src/hand_renderer.cpp \
	src/glut_backend.hpp \
	src/glut_backend.cpp \
	src/pipeline.hpp \
	src/pipeline.cpp \
	src/shader.hpp \
	src/shader.cpp \
	src/mesh.hpp \
	src/mesh.cpp \
	src/hand.hpp \
	src/hand.cpp \
	src/cone.hpp \
	Makefile
		g++ -o $@ $(filter %.cpp,$^) $(CFLAGS) 

clean:
		rm -rf *o hand_renderer
