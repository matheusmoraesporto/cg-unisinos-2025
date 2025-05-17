#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord_in;
uniform mat4 model;
out vec2 texcoord;
void main()
{
   gl_Position = model * vec4(position, 1.0);
   texcoord = texcoord_in;
}
