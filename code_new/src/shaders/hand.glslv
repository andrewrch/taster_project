#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in mat4 WVP;
layout (location = 6) in mat4 WV;
    
out vec4 projPos;
out vec4 colour;
flat out int instanceID; 

void main()
{
  gl_Position = projPos = WVP * vec4(position, 1.0);
  //gl_Position = vec4(position, 1.0);
  colour = vec4(clamp(position, 0.0, 1.0), 1.0);
  instanceID = gl_InstanceID;  
}
