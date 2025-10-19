#ifndef INPUT_H
#define INPUT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Input
{
public:
	Input();

	void ProcessInput(GLFWwindow* window);
};

#endif
