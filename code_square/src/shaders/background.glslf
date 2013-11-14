#version 410

in vec2 textureCoords;
out vec3 colour;

uniform isampler2D textureSampler;

void main()
{
  colour = vec3( texture2D(textureSampler, textureCoords).r, 0, 0) ;
}
