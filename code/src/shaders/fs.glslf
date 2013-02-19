#version 330

flat in int instanceID;  
in vec4 colour;
out vec4 fragColour;

void main()
{
  //if (instanceID == 0)
  //  fragColour = vec4(0.0, 1.0, 0.0, 1.0);
  //else
  //  fragColour = vec4(1.0, 0.0, 0.0, 1.0);
  fragColour = colour;

  //fragColour = vec4(1.0, 0.0, 0.0, 1.0);
}
