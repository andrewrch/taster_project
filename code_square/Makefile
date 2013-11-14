CFLAGS := -O4 -g -Wall -std=c++0x -I/opt/AMDAPP/include 
LDFLAGS := -lm -lGL -lGLEW -lglfw -lglut -lOpenCL `pkg-config opencv --libs` -lboost_filesystem -lboost_system

all: hand_renderer

hand_renderer:\
	src/classifier.cpp\
	src/classifier.hpp\
	src/cone.hpp\
	src/hand.cpp\
	src/hand.hpp\
	src/hand_renderer.cpp\
	src/histogram.cpp\
	src/histogram.hpp\
	src/mesh.cpp\
	src/mesh.hpp\
	src/pipeline.cpp\
	src/pipeline.hpp\
	src/shader.cpp\
	src/shader.hpp\
	src/scorer.hpp\
	src/scorer.cpp\
	src/thresholder.cpp\
	src/thresholder.hpp\
	src/particle.cpp\
	src/particle.hpp\
	src/particleswarm.hpp\
	src/particleswarm.cpp\
	Makefile
		g++ -o $@ $(filter %.cpp,$^) $(CFLAGS) $(LDFLAGS)

model_viewer:\
 	src/model_viewer.cpp \
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
		g++ -o $@ $(filter %.cpp,$^) $(CFLAGS) $(LDFLAGS) 



clean:
		rm -rf *o hand_renderer
