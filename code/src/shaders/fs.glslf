#version 410

flat in int instanceID;  
in vec4 colour;
out vec4 fragColour;

void main()
{
  fragColour = colour;
}
