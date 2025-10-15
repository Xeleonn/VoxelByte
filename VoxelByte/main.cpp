#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "clock.h"
#include "shader.h"
#include "player.h"
#include "voxel.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void updateImGui();

// Initialize clock
Clock game_clock;

// Initialize voxel system
Voxel voxel;

// Settings
const unsigned int WINDOW_WIDTH = 1920;
const unsigned int WINDOW_HEIGHT = 1080;
bool wireframeMode = false;
float clearColor[3] = { 0.7f, 0.7f, 1.0f };

// Player
Player player;

// Camera
std::shared_ptr<Camera> pCamera = std::make_shared<Camera>(glm::vec3(Chunk::CHUNK_SIZE / 2, Chunk::CHUNK_SIZE / 2, Chunk::CHUNK_SIZE * 1.5f + 100.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;
float viewDistance = 1000.0f;
float cameraSpeed = pCamera->MovementSpeed;

int main() {

    // GLFW: Initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "VoxelByte", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSwapInterval(0);

    // GLAD: Load all OpenGL function pointers
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    io.IniFilename = nullptr;

    // Vertex shader
    const char* vertexShaderSource
    {
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0"
    };

    // Fragment shader
    const char* fragmentShaderSource
    {
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "\n"
    "void main() {\n"
    "    // Get normalized Y position (0.0 = bottom, 1.0 = top)\n"
    "    float gradient = gl_FragCoord.y / 1080.0; \n" // adjust to window height
    "\n"
        // Make the gradient darker at bottom, normal at top
    "    float shade = mix(0.6, 1.0, gradient);\n"
    "\n"
    "    FragColor = vec4(ourColor * shade, 1.0f);\n"
    "}\n\0"
    };

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE); // Optional: enable face culling if confident in winding order
    // glCullFace(GL_BACK);

    Shader ourShader(vertexShaderSource, fragmentShaderSource);

    // Test chunk generation and mesh setup
    Chunk test_chunk = voxel.GenerateTestChunk();
    Voxel::VoxelMesh test_mesh = voxel.GenerateChunkMesh2(test_chunk);

    GLuint chunkVBO = 0, chunkEBO = 0, chunkVAO = 0; // Initialize to 0
    voxel.SetupRenderMesh(test_mesh, chunkVBO, chunkEBO, chunkVAO);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        game_clock.Update();
        processInput(window);

        glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ourShader.use();

        // Pass projection matrix to shader
        glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, viewDistance);
        unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Camera/view transformation
        glm::mat4 view = pCamera->GetViewMatrix();
        unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        pCamera->MovementSpeed = cameraSpeed;

        voxel.RenderMesh(test_mesh, chunkVAO);

        updateImGui();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    if (chunkVAO != 0) {
        voxel.FreeRenderMesh(test_mesh, chunkVBO, chunkEBO, chunkVAO);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    // Control cursor visibility with Right Mouse Button
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstMouse = true;
        }
    }

    // Quit program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    float deltaTime = game_clock.GetDeltaTime();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pCamera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pCamera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        pCamera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        pCamera->ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        pCamera->ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        pCamera->ProcessKeyboard(DOWN, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    // Only process mouse if cursor is disabled
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        pCamera->ProcessMouseMovement(xoffset, yoffset);
    }
    else { // If cursor is normal, reset firstMouse so movement isn't jerky when re-disabling
        firstMouse = true;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        pCamera->ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

void updateImGui() {
    ImGui::SetNextWindowSize(ImVec2(305, 300));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
    ImGui::Text("Time: %.2f s", game_clock.GetTime());
    ImGui::Text("Delta Time: %.4f ms", game_clock.GetDeltaTime() * 1000.0f); // ms
    ImGui::Text("%.1f FPS", game_clock.GetFPS());

    ImGui::Separator();
    ImGui::Text("Camera Position:");
    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z);
    ImGui::Text("Yaw: %.1f, Pitch: %.1f", pCamera->Yaw, pCamera->Pitch);

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Window")) {
        ImGui::Checkbox("Wireframe Meshes", &wireframeMode);
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        ImGui::Separator();
        ImGui::Text("glClearColor:");
        ImGui::ColorEdit3("Color", clearColor);
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::Separator();
        ImGui::Text("Speed:");
        ImGui::SliderFloat(" ", &cameraSpeed, 1.0f, 1000.0f);
        ImGui::Separator();
        ImGui::Text("View Distance:");
        ImGui::SliderFloat("  ", &viewDistance, 100.0f, 10000.0f);
        ImGui::Spacing();
    }
    ImGui::End();
}