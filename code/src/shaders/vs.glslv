#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in mat4 WVP;
    
//out vec4 colour;
//flat out int instanceID; 

void main()
{
  gl_Position = vec4(0.5 * position.x, 0.5 * position.y, position.z, 1.0);
//   gl_Position = vec4(position, 1.0);
//    colour = vec4(clamp(position, 0.0, 1.0), 1.0);
//    instanceID = gl_InstanceID;  
}
