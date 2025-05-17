#version 410 core
in vec2 texcoord;
out vec4 color;
uniform sampler2D texbuffer;
void main()
{
   color = texture(texbuffer, texcoord);
}
