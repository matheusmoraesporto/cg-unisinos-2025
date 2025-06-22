#version 410
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texc;
layout (location = 2) in vec3 normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec2 texCoord;
out vec3 vNormal;
out vec4 fragPos; 
void main()
{
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
    fragPos = model * vec4(position.x, position.y, position.z, 1.0);
    texCoord = vec2(texc.x, 1-texc.y);
    vNormal = mat3(transpose(inverse(model))) * normal;
}
