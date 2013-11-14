//A simple class for handling  shader compilation
//Author: Movania Muhammad Mobeen
//Last Modified: February 2, 2011

#include <iostream>
#include <fstream>
#include <assert.h>

#include "shader.hpp"

Shader::Shader(void)
{
  totalShaders = 0;
  shaders[VERTEX_SHADER] = 0;
  shaders[FRAGMENT_SHADER] = 0;
  shaders[GEOMETRY_SHADER] = 0;
  attributeList.clear();
  uniformLocationList.clear();
}

Shader::~Shader(void)
{
  attributeList.clear();
  uniformLocationList.clear();  
}

void Shader::loadFromFile(GLenum shaderType, const string& filename){
  ifstream fp;
  fp.open(filename.c_str(), ios_base::in);
  if(fp) {    
    string buffer(std::istreambuf_iterator<char>(fp), (std::istreambuf_iterator<char>()));
    //copy to source
    loadFromString(shaderType, buffer);   
  } 
  else 
    cerr<<"Error loading shader: "<<filename<<endl;
}


void Shader::loadFromString(GLenum type, const string& source) { 
  GLuint shader = glCreateShader (type);

  const char * ptmp = source.c_str();
  glShaderSource (shader, 1, &ptmp, NULL);
    
  //check whether the shader loads fine
  GLint status;
  glCompileShader (shader);
  glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;   
    glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *infoLog= new GLchar[infoLogLength];
    glGetShaderInfoLog (shader, infoLogLength, NULL, infoLog);
    cerr<<"Compile log: "<<infoLog<<endl;
    delete [] infoLog;
  }
  shaders[totalShaders++]=shader;
}

void Shader::createAndLinkProgram() {
  program = glCreateProgram ();
  if (shaders[VERTEX_SHADER] != 0) {
    glAttachShader (program, shaders[VERTEX_SHADER]);
  }
  if (shaders[FRAGMENT_SHADER] != 0) {
    glAttachShader (program, shaders[FRAGMENT_SHADER]);
  }
  if (shaders[GEOMETRY_SHADER] != 0) {
    glAttachShader (program, shaders[GEOMETRY_SHADER]);
  }
    
  //link and check whether the program links fine
  GLint status;
  glLinkProgram (program);
  glGetProgramiv (program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) 
  {
    GLint infoLogLength;
      
    glGetProgramiv (program, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *infoLog= new GLchar[infoLogLength];
    glGetProgramInfoLog (program, infoLogLength, NULL, infoLog);
    cerr<<"Link log: "<<infoLog<<endl;
    delete [] infoLog;
  }

  glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
  if (status == GL_FALSE) 
  {
    GLint infoLogLength;
    glGetProgramiv (program, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *infoLog= new GLchar[infoLogLength];
    glGetProgramInfoLog (program, infoLogLength, NULL, infoLog);
    cerr<<"Validation log: "<<infoLog<<endl;
    delete [] infoLog;
  }

  glDeleteShader(shaders[VERTEX_SHADER]);
  glDeleteShader(shaders[FRAGMENT_SHADER]);
  glDeleteShader(shaders[GEOMETRY_SHADER]);
}

void Shader::use() {
  glUseProgram(program);
}

void Shader::unUse() {
  glUseProgram(0);
}

void Shader::addAttribute(const string& attribute) {
  attributeList[attribute]= glGetAttribLocation(program, attribute.c_str()); 
}

//An indexer that returns the location of the attribute
GLuint Shader::operator [](const string& attribute) {
  return attributeList[attribute];
}

GLuint Shader::addUniform(const string& uniform) {
  uniformLocationList[uniform] = glGetUniformLocation(program, uniform.c_str());
  //assert(uniformLocationList[uniform] != 0xFFFFFFFF);
  return uniformLocationList[uniform];
}

GLuint Shader::operator()(const string& uniform){
  return uniformLocationList[uniform];
}
GLuint Shader::getProgram() const {
  return program;
}


