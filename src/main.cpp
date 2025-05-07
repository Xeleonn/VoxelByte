#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "clock.h"
#include "voxel.h"
#include "camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Initialize clock
Clock game_clock;

Voxel voxel;

int main()
{
    // GLFW: Initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "VoxelByte", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD: Load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Vertex shader
    const char *vertexShaderSource
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
    const char *fragmentShaderSource
    {
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main() {\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0"
    };

    float vertices[] = {
        // positions          // colors
        // Front face (Red)
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // bottom-left
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // top-right
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // top-left
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // bottom-left
        // Back face (Green)
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // top-right
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-left
        // Left face (Blue)
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // top-front
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, // top-back
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f, // bottom-back
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f, // bottom-back
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // bottom-front
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // top-front
        // Right face (Yellow)
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // top-front
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, // top-back
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, // bottom-back
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, // bottom-back
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // bottom-front
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // top-front
        // Bottom face (Cyan)
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // back-left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // back-right
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // front-right
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // front-right
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // front-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // back-left
        // Top face (Magenta)
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // back-left
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // back-right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, // front-right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, // front-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, // front-left
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f  // back-left
    };

    // Configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // Build and compile our shader program
    Shader ourShader(vertexShaderSource, fragmentShaderSource);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
    // Update clock
    game_clock.Update();

    // Input
    processInput(window);

    // Render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Activate shader
    ourShader.use();

    // Pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Camera/view transformation
    glm::mat4 view = camera.GetViewMatrix();
    unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // Render boxes
    glBindVertexArray(VAO);
    voxel.DrawVoxel(91, ourShader);

    // ImGui window elements
    ImGui::SetNextWindowSize(ImVec2(200, 300));
    ImGui::SetNextWindowPos(ImVec2(25, 25));
    ImGui::Begin("VoxelByte", nullptr, ImGuiWindowFlags_NoSavedSettings);
    ImGui::Text("Time: %.2f", game_clock.GetTime());
    ImGui::Text("Delta Time: %lf", game_clock.GetDeltaTime());
    ImGui::Text("Fps: %.2f", game_clock.GetFPS());
    ImGui::Text("Voxels generated: %d", voxel.GetVoxelsGenerated());
    ImGui::End();

    // ImGui render
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // GLFW: swap buffers and poll IO events. (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
    }

    // Terminate ImGui resources.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Optional: de-allocates all resources once they've outlived their purpose.
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // GLFW: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, game_clock.GetDeltaTime());
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, game_clock.GetDeltaTime());
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, game_clock.GetDeltaTime());
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, game_clock.GetDeltaTime());
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, game_clock.GetDeltaTime());
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, game_clock.GetDeltaTime());
}

// GLFW: Whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// GLFW: Whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// GLFW: Whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}