#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in mat4 WVP;
layout (location = 6) in mat4 WV;
    
out vec4 projPos;
out vec4 colour;
varying float depth;
flat out int instanceID; 

void main()
{
  gl_Position = projPos = WVP * vec4(position, 1.0);
  vec4 d = (WV * vec4(position, 1.0));
  depth = -d.z;
  colour = vec4(clamp(position, 0.0, 1.0), 1.0);
  instanceID = gl_InstanceID;  
}
