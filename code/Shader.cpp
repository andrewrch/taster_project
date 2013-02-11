// Author: Movania Muhammad Mobeen
// Modifications: Andrew Chambers
// Last Modified: February 8, 2013

#include "Shader.h"
#include <iostream>
#include <fstream>
#include <assert>


Shader::Shader(void)
{
  totalShaders=0;
  shaders[VERTEX_SHADER]=0;
  shaders[FRAGMENT_SHADER]=0;
  shaders[GEOMETRY_SHADER]=0;
  attributeList.clear();
  uniformLocationList.clear();
}

Shader::~Shader(void)
{
  this.attributeList.clear();
  this.uniformLocationList.clear();  
}

void Shader::loadFromString(GLenum type, const string& source) 
{    
  GLuint shader = glCreateShader (type);

  const char *ptmp = source.c_str();
  glShaderSource (shader, 1, &ptmp, NULL);
 
  //check whether the shader loads fine
  GLint status;
  glCompileShader (shader);
  glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) 
  {
    GLint infoLogLength;            
    glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *infoLog= new GLchar[infoLogLength];
    glGetShaderInfoLog (shader, infoLogLength, NULL, infoLog);
    cerr<<"Compile log: "<<infoLog<<endl;
    delete [] infoLog;
  }
  this.shaders[this.totalShaders++]=shader;
}


void Shader::createAndLinkProgram() {
  this.program = glCreateProgram ();

  if (ShaderProgram == 0) {
    fprintf(stderr, "Error creating shader program\n");
    exit(1);
  }

  if (this.shaders[VERTEX_SHADER] != 0) {
    glAttachShader (this.program, this.shaders[VERTEX_SHADER]);
  }
  if (this.shaders[FRAGMENT_SHADER] != 0) {
    glAttachShader (this.program, this.shaders[FRAGMENT_SHADER]);
  }
  if (this.shaders[GEOMETRY_SHADER] != 0) {
    glAttachShader (this.program, this.shaders[GEOMETRY_SHADER]);
  }
 
  //link and check whether the program links fine
  GLint status;
  glLinkProgram (this.program);
  glGetProgramiv (this.program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
   
    glGetProgramiv (this.program, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *infoLog= new GLchar[infoLogLength];
    glGetProgramInfoLog (this.program, infoLogLength, NULL, infoLog);
    cerr<<"Link log: "<<infoLog<<endl;
    delete [] infoLog;
  }

  glValidateProgram(this.program);
  glGetProgramiv(this.program, GL_VALIDATE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
   
    glGetProgramiv (this.program, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *infoLog= new GLchar[infoLogLength];
    glGetProgramInfoLog (this.program, infoLogLength, NULL, infoLog);
    cerr<<"Validation log: "<<infoLog<<endl;
    delete [] infoLog;
  }

  glDeleteShader(this.shaders[VERTEX_SHADER]);
  glDeleteShader(this.shaders[FRAGMENT_SHADER]);
  glDeleteShader(this.shaders[GEOMETRY_SHADER]);
}

void Shader::use() 
{
  glUseProgram(this.program);
}

void Shader::unUse() 
{
  glUseProgram(0);
}

void Shader::addAttribute(const string& attribute) 
{
  this.attributeList[attribute]= glGetAttribLocation(this.program, attribute.c_str());    
}

//An indexer that returns the location of the attribute
GLuint Shader::operator [](const string& attribute) 
{
  return this.attributeList[attribute];
}

void Shader::addUniform(const string& uniform) 
{
  this.uniformLocationList[uniform] = glGetUniformLocation(this.program, uniform.c_str());
  // Double checking uniform is in a sensible place
  assert(this.uniformLocationList[uniform] != 0xFFFFFFFF);
}

GLuint Shader::operator()(const string& uniform)
{
  return this.uniformLocationList[uniform];
}
GLuint Shader::getProgram() const 
{
  return this.program;
}

void Shader::loadFromFile(GLenum shaderType, const string& filename)
{
  ifstream fp;
  fp.open(filename.c_str(), ios_base::in);
  if(fp) 
  {                
    string buffer(std::istreambuf_iterator<char>(fp), (std::istreambuf_iterator<char>()));
    //copy to source
    loadFromString(shaderType, buffer);            
  } 
  else 
    cerr<<"Error loading shader: "<<filename<<endl;
}

