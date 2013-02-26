CFLAGS := -g -Wall
LDFLAGS := -lm -lGL -lGLEW -lglut `pkg-config opencv --libs` 

all: hand_renderer kinect_viewer

hand_renderer:\
	src/blob.hpp\
	src/callbacks.h\
	src/classifier.cpp\
	src/classifier.hpp\
	src/componentlabeller.cpp\
	src/componentlabeller.hpp\
	src/cone.hpp\
	src/disjointset.cpp\
	src/disjointset.hpp\
	src/glut_backend.cpp\
	src/glut_backend.hpp\
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
	src/thresholder.cpp\
	src/thresholder.hpp\
	src/util.h\
	Makefile
		g++ -o $@ $(filter %.cpp,$^) $(CFLAGS) $(LDFLAGS)

kinect_viewer:\
	src/blob.hpp\
	src/callbacks.h\
	src/classifier.cpp\
	src/classifier.hpp\
	src/componentlabeller.cpp\
	src/componentlabeller.hpp\
	src/cone.hpp\
	src/disjointset.cpp\
	src/disjointset.hpp\
	src/glut_backend.cpp\
	src/glut_backend.hpp\
	src/hand.cpp\
	src/hand.hpp\
	src/histogram.cpp\
	src/histogram.hpp\
	src/mesh.cpp\
	src/mesh.hpp\
	src/pipeline.cpp\
	src/pipeline.hpp\
	src/shader.cpp\
	src/shader.hpp\
	src/thresholder.cpp\
	src/thresholder.hpp\
	src/util.h\
	src/kinect_viewer.cpp \
	src/depthmasker.cpp \
	src/depthmasker.hpp \
	Makefile
		g++ -o $@ $(filter %.cpp,$^) $(CFLAGS) $(LDFLAGS)

# model_viewer: \
# 	src/model_viewer.cpp \
# 	src/glut_backend.hpp \
# 	src/glut_backend.cpp \
# 	src/pipeline.hpp \
# 	src/pipeline.cpp \
# 	src/shader.hpp \
# 	src/shader.cpp \
# 	src/mesh.hpp \
# 	src/mesh.cpp \
# 	src/hand.hpp \
# 	src/hand.cpp \
# 	src/cone.hpp \
# 	Makefile
# 		g++ -o $@ $(filter %.cpp,$^) $(CFLAGS) 
# 
# 
# 

clean:
		rm -rf *o hand_renderer
