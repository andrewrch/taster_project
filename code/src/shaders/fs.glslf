#version 330

flat in int instanceID;  
in vec4 projPos;
out vec4 fragColour;

void main()
{
  //float z = (projPos.z + 1.0f) / 2.0f;
  //float z = (projPos.z/projPos.w + 1.0f) / 2.0f;
  float z = projPos.z/projPos.w;
  float n = 1.0f; // camera z near
  float f = 100.0f; // camera z far
  float z_e = (n * z) / ( f - z * (f - n) );
  fragColour = vec4(1 - z_e, 1 - z_e, 1 - z_e, 1.0); //colour;
}
