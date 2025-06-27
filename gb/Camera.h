#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Camera
{
public:
	Camera() {}
	~Camera() {}
	void initialize(
		Shader *shader,
		int width,
		int height,
		float sensitivity,
		float pitch,
		float yaw,
		glm::vec3 cameraFront,
		glm::vec3 cameraPos,
		glm::vec3 cameraUp);
	void move(GLFWwindow *window, int key, int action);
	void rotate(GLFWwindow *window, double xpos, double ypos);
	void update();
	bool isSelected() { return selected; };
	void select() { selected = true; };
	void unselect() { selected = false; };

protected:
	Shader *shader;
	bool firstMouse, rotateX, rotateY, rotateZ;
	float lastX, lastY, pitch, yaw;
	float sensitivity;
	glm::vec3 cameraFront, cameraPos, cameraUp;
	bool selected;
};
