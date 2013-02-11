//A simple class for handling GLSL shader compilation
//Auhtor: Movania Muhammad Mobeen
#pragma once
#include <GL/glew.h>
#include <map>
#include <string>

using namespace std;

class Shader
{
  public:
    Shader(void);
    ~Shader(void);      
    void loadFromString(GLenum whichShader, const string& source);
    void loadFromFile(GLenum whichShader, const string& filename);
    void createAndLinkProgram();
    void use();
    void unUse();
    void addAttribute(const string& attribute);
    GLuint addUniform(const string& uniform);
    GLuint getProgram() const;
    //An indexer that returns the location of the attribute/uniform
    GLuint operator[](const string& attribute);
    GLuint operator()(const string& uniform);
    //Program deletion
    void deleteProgram() 
    {
      glDeleteProgram(this->program);
      this->program=-1;
    }
  private:
    enum ShaderType {VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER};
    GLuint program;
    int totalShaders;
    GLuint shaders[3];//0->vertexshader, 1->fragmentshader, 2->geometryshader
    map<string,GLuint> attributeList;
    map<string,GLuint> uniformLocationList;
};      


