#ifndef INPUT_H
#define INPUT_H

#include "globals.h"
#include "clock.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Input
{
public:
	Input();

	void ProcessInput(GLFWwindow* window, Clock gameClock);
};

#endif
