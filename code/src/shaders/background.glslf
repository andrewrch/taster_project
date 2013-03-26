#version 410

in vec2 textureCoords;
out vec3 colour;

uniform sampler2D textureSampler;

void main()
{
  colour = texture2D(textureSampler, textureCoords).rgb;
}
