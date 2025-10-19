#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "globals.h"

#include "clock.h"
#include "shader.h"
#include "player.h"
#include "voxel.h"
#include "window.h"
#include "gui.h"
#include "logger.h"
#include "input.h"

#include <iostream>
#include <unordered_map>

// Initialize clock
Clock gameClock;

// Initialize voxel system
Voxel voxel;

// Settings
float clearColor[3] = { 0.7f, 0.7f, 1.0f };

// Player
Player player;

// Camera
float viewDistance = 1000.0f;

// Input
Input input;

// GUI
GUI gui(camera, gameClock);

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
    "uniform vec3 pos_offset;\n"
    "void main() {\n"
    "   gl_Position = projection * view * model * vec4(aPos + pos_offset, 1.0);\n"
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

    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    Shader ourShader(vertexShaderSource, fragmentShaderSource);
    Shader crosshairShader(gui.crosshairVertexShaderSource, gui.crosshairFragmentShaderSource);

    // Setup crosshair
    gui.SetupCrosshairMesh();

    // Test chunk generation and mesh setup
    //Chunk testChunk(0, glm::ivec3(0, 0, 0));
    //testChunk.GenerateChunk();
    //Voxel::VoxelMesh testMesh = voxel.GenerateChunkMesh2(testChunk);
    ChunkSystem chunk_sys;
    chunk_sys.update();

    int iterator = 5226;
    std::vector<Voxel::VoxelMesh> mesh_vec;
    for (const auto& id_chunk : chunk_sys.get_chunk_map())
    {
        Voxel::VoxelMesh curr_mesh = voxel.GenerateChunkMesh2(*(id_chunk.second));
        mesh_vec.push_back(curr_mesh);
        curr_mesh.VBO = iterator;
        curr_mesh.EBO = iterator;
        curr_mesh.VAO = iterator;
        curr_mesh.mesh_offset = glm::vec3(  static_cast<float>(id_chunk.second->getOrigin().x),
                                            static_cast<float>(id_chunk.second->getOrigin().y),
                                            static_cast<float>(id_chunk.second->getOrigin().z));
        voxel.SetupRenderMesh(curr_mesh, curr_mesh.VBO, curr_mesh.EBO, curr_mesh.VAO);
        iterator++;
    }

    GLuint chunkVBO = 0, chunkEBO = 0, chunkVAO = 0; // Initialize to 0

    //gui.SetupCrosshairMesh(gui.crosshairVAO, gui.crosshairVBO, gui.crosshairEBO);

    // Render loop
    while (!window.ShouldClose()) {
        gameClock.Update();
        input.ProcessInput(window.GetGLFWwindow(), gameClock);

        glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ourShader.use();

        // Projection
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)window.GetWidth() / (float)window.GetHeight(),
            0.1f, gui.GetViewDistance());
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));

        // View
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"),
            1, GL_FALSE, glm::value_ptr(view));

        // Model
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"),
            1, GL_FALSE, glm::value_ptr(model));


        for (const Voxel::VoxelMesh& mesh : mesh_vec)
        {
            glUniform3f(glGetUniformLocation(ourShader.ID, "pos_offset"),
                        mesh.mesh_offset.x,
                        mesh.mesh_offset.y,
                        mesh.mesh_offset.z);
                        
            voxel.RenderMesh(mesh, mesh.VAO);
        }

        // --- Draw crosshair ---
        glDisable(GL_DEPTH_TEST);
        crosshairShader.use();
        glBindVertexArray(gui.crosshairVAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);

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