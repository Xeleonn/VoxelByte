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
#include "window.h"
#include "gui.h"

#include <iostream>
#include <unordered_map>

// Function declarations
void processInput(GLFWwindow* window);

// Initialize clock
Clock gameClock;

// Initialize voxel system
Voxel voxel;

// Settings
float clearColor[3] = { 0.7f, 0.7f, 1.0f };

// Player
Player player;

// Camera
std::shared_ptr<Camera> pCamera = std::make_shared<Camera>(glm::vec3(Chunk::CHUNK_SIZE / 2, Chunk::CHUNK_SIZE / 2, Chunk::CHUNK_SIZE * 1.5f + 100.0f));
float viewDistance = 1000.0f;
float cameraSpeed = pCamera->MovementSpeed;

// GUI
GUI gui(pCamera, gameClock);

int main() {

    // Create window
    Window window(1920, 1080, "VoxelByte");

    // Initialize ImGUI
    gui.InitializeImGUI(window.GetGLFWwindow());

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

    Shader ourShader(vertexShaderSource, fragmentShaderSource);

    // Test chunk generation and mesh setup
    //Chunk testChunk(0, glm::ivec3(0, 0, 0));
    //testChunk.GenerateChunk();
    //Voxel::VoxelMesh testMesh = voxel.GenerateChunkMesh2(testChunk);
    ChunkSystem chunk_sys(pCamera);
    chunk_sys.update();

    int iterator = 0;
    std::vector<Voxel::VoxelMesh> mesh_vec;
    for (const auto& id_chunk : chunk_sys.get_chunk_map())
    {
        Voxel::VoxelMesh curr_mesh = voxel.GenerateChunkMesh2(*(id_chunk.second));
        mesh_vec.push_back(curr_mesh);
        curr_mesh.VBO = iterator;
        curr_mesh.EBO = iterator;
        curr_mesh.VAO = iterator;
        voxel.SetupRenderMesh(curr_mesh, curr_mesh.VBO, curr_mesh.EBO, curr_mesh.VAO);
        iterator++;
    }

    GLuint chunkVBO = 0, chunkEBO = 0, chunkVAO = 0; // Initialize to 0

    // Render loop
    while (!window.ShouldClose()) {
        gameClock.Update();
        processInput(window.GetGLFWwindow());

        glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ourShader.use();

        // Projection
        glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom),
            (float)window.GetWidth() / (float)window.GetHeight(),
            0.1f, gui.GetViewDistance());
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));

        // View
        glm::mat4 view = pCamera->GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"),
            1, GL_FALSE, glm::value_ptr(view));

        // Model
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"),
            1, GL_FALSE, glm::value_ptr(model));

        pCamera->MovementSpeed = cameraSpeed;

        for (const Voxel::VoxelMesh& mesh : mesh_vec)
        voxel.RenderMesh(mesh, mesh.VAO);

        gui.UpdateImGui(window.GetGLFWwindow());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.SwapBuffers();
        window.PollEvents();
    }

    // Cleanup
    for (const Voxel::VoxelMesh& mesh : mesh_vec)
        voxel.FreeRenderMesh(mesh, const_cast<GLuint&>(mesh.VBO), const_cast<GLuint&>(mesh.EBO), const_cast<GLuint&>(mesh.VAO));
    

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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
        }
    }

    // Quit program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    float deltaTime = float(gameClock.GetDeltaTime());
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