#ifndef GUI_H
#define GUI_H

#include "globals.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <memory>
#include "window.h"
#include "clock.h"
#include "camera.h"

class GUI
{
public:
    GUI(Camera& camera, Clock& clock);
    ~GUI();

    void InitializeImGUI(GLFWwindow* window);
    void UpdateImGui(GLFWwindow* window);
    float GetViewDistance();

    // GUI state
    bool wireFrameMode = false;
    float viewDistance = 1000.0f;
    float cameraSpeed = 100.0f;

    void SetupCrosshairMesh();

    const char* crosshairVertexShaderSource;
    const char* crosshairFragmentShaderSource;

    GLuint crosshairVAO, crosshairVBO, crosshairEBO;

private:
    Clock& gameClock;
};

#endif
