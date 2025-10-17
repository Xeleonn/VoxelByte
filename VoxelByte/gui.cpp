#include "gui.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GUI::GUI(std::shared_ptr<Camera> camera, Clock& clock) : pCamera(camera), gameClock(clock)
{
}

GUI::~GUI() {}

void GUI::InitializeImGUI(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    io.IniFilename = nullptr;
}

void GUI::UpdateImGui(GLFWwindow* window)
{
    ImGui::SetNextWindowSize(ImVec2(305, 1440));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);

    ImGui::Text("Time: %.2f s", gameClock.GetTime());
    ImGui::Text("Delta Time: %.4f ms", gameClock.GetDeltaTime() * 1000.0f);
    ImGui::Text("%.1f FPS", gameClock.GetFPS());

    ImGui::Separator();
    ImGui::Text("Camera Position:");
    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z);
    ImGui::Text("Yaw: %.1f, Pitch: %.1f", pCamera->Yaw, pCamera->Pitch);

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Window")) {
        const char* items[] = { "1280x720", "1600x900", "1920x1080", "2560x1140" };
        static int currentItem = 2;
        ImGui::Text("Window Size:");
        if (ImGui::Combo("   ", &currentItem, items, IM_ARRAYSIZE(items)))
        {
            if (currentItem == 0)
                glfwSetWindowSize(window, 1280, 720);
            else if (currentItem == 1)
                glfwSetWindowSize(window, 1600, 900);
            else if (currentItem == 2)
                glfwSetWindowSize(window, 1920, 1080);
            else if (currentItem == 3)
                glfwSetWindowSize(window, 2560, 1440);
        }

        ImGui::Separator();
        ImGui::Checkbox(" Wireframe Meshes", &wireFrameMode);
        if (wireFrameMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::Separator();
        ImGui::Text("Speed:");
        ImGui::SliderFloat(" ", &cameraSpeed, 1.0f, 1000.0f);
        pCamera->MovementSpeed = cameraSpeed;
        ImGui::Separator();
        ImGui::Text("View Distance:");
        ImGui::SliderFloat("  ", &viewDistance, 100.0f, 10000.0f);
        ImGui::Spacing();
    }

    ImGui::End();
}

float GUI::GetViewDistance()
{
    return viewDistance;
}