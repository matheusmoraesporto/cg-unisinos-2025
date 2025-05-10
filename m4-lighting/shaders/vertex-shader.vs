#version 410
layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 texc;

uniform mat4 projection;
uniform mat4 model;

out vec2 texCoord;
out vec3 vNormal;
out vec4 fragPos; 
void main()
{
	gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);
    fragPos = model * vec4(position.x, position.y, position.z, 1.0);
    texCoord = vec2(texc.x, 1-texc.y);
    vNormal = normal;
}
