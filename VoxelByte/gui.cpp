#include "globals.h"

GUI::GUI()
{
}

float crosshairVertices[] = {
    // Vertical bar (slightly longer to match horizontal visually)
    -0.001f,  0.017f, 0.0f,
     0.001f,  0.017f, 0.0f,
     0.001f, -0.017f, 0.0f,
    -0.001f, -0.017f, 0.0f,

    // Horizontal bar (base size)
    -0.015f,  0.001f, 0.0f,
     0.015f,  0.001f, 0.0f,
     0.015f, -0.001f, 0.0f,
    -0.015f, -0.001f, 0.0f
};

unsigned int crosshairIndices[] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};

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

    VB::inst().GetLogger()->Print("ImGUI initialized");
}

void GUI::UpdateImGui(GLFWwindow* window)
{
    ImGui::SetNextWindowSize(ImVec2(305, 1440));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);

    ImGui::Text("Time: %.2f s", VB::inst().GetClock()->GetTime());
    ImGui::Text("Delta Time: %.4f ms", VB::inst().GetClock()->GetDeltaTime() * 1000.0f);
    ImGui::Text("%.1f FPS", VB::inst().GetClock()->GetFPS());

    ImGui::Separator();
    ImGui::Text("Camera Position:");
    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", VB::inst().GetCamera()->Position.x, VB::inst().GetCamera()->Position.y, VB::inst().GetCamera()->Position.z);
    ImGui::Text("Yaw: %.1f, Pitch: %.1f", VB::inst().GetCamera()->Yaw, VB::inst().GetCamera()->Pitch);

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
        VB::inst().GetCamera()->MovementSpeed = cameraSpeed;
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

void GUI::SetupCrosshairMesh()
{
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glGenBuffers(1, &crosshairEBO);

    glBindVertexArray(crosshairVAO);

    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crosshairEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(crosshairIndices), crosshairIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}