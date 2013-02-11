CFLAGS := -Wall -lm -lGL -lGLEW -lglut -DGL_GLEXT_PROTOTYPES

all: draw

draw: \
	src/main.cpp \
	src/camera.cpp \
	src/camera.h \
	src/math_3d.cpp \
	src/math_3d.h \
	src/pipeline.cpp \
	src/pipeline.h \
	Makefile
		g++ -o $@ $(filter %.cpp,$^) $(CFLAGS) 
