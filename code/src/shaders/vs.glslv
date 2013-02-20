#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in mat4 WVPTiled;
layout (location = 6) in mat4 WVP;
    
out vec4 projPos;
flat out int instanceID; 

void main()
{
  gl_Position = WVPTiled * vec4(position, 1.0);
  projPos = WVPTiled * vec4(position, 1.0);
  instanceID = gl_InstanceID;  
}
