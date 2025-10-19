#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "globals.h"

#include "shader.h"
#include "window.h"

#include <iostream>
#include <unordered_map>

int main() {

    // Initialize Global Class
    VB::inst().init();

    // Create window
    Window window(1920, 1080, "VoxelByte");

    // Initialize ImGUI
    VB::inst().GetGUI()->InitializeImGUI(window.GetGLFWwindow());

    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    Shader ourShader("../shaders/voxel_vert.glsl", "../shaders/voxel_frag.glsl");
    Shader crosshairShader("../shaders/crosshair_vert.glsl", "../shaders/crosshair_frag.glsl");

    // Setup crosshair
    VB::inst().GetGUI()->SetupCrosshairMesh();

    // Test chunk generation and mesh setup
    //Chunk testChunk(0, glm::ivec3(0, 0, 0));
    //testChunk.GenerateChunk();
    //Voxel::VoxelMesh testMesh = voxel.GenerateChunkMesh2(testChunk);
    MultiChunkSystem chunk_sys;
    chunk_sys.update();

    int iterator = 5226;
    std::vector<Voxel::VoxelMesh> mesh_vec;
    for (const auto& id_chunk : chunk_sys.get_chunk_map())
    {
        Voxel::VoxelMesh curr_mesh = VB::inst().GetVoxel()->GenerateChunkMesh2(*(id_chunk.second));
        mesh_vec.push_back(curr_mesh);
        curr_mesh.VBO = iterator;
        curr_mesh.EBO = iterator;
        curr_mesh.VAO = iterator;
        curr_mesh.mesh_offset = glm::vec3(  static_cast<float>(id_chunk.second->getOrigin().x),
                                            static_cast<float>(id_chunk.second->getOrigin().y),
                                            static_cast<float>(id_chunk.second->getOrigin().z));
                                            
        VB::inst().GetVoxel()->SetupRenderMesh(curr_mesh, curr_mesh.VBO, curr_mesh.EBO, curr_mesh.VAO);
        iterator++;
    }

    GLuint chunkVBO = 0, chunkEBO = 0, chunkVAO = 0; // Initialize to 0

    //gui.SetupCrosshairMesh(gui.crosshairVAO, gui.crosshairVBO, gui.crosshairEBO);

    // Render loop
    while (!window.ShouldClose()) {
        VB::inst().GetClock()->Update();
        VB::inst().GetInput()->ProcessInput(window.GetGLFWwindow());

        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ourShader.use();

        // Projection
        glm::mat4 projection = glm::perspective(glm::radians(VB::inst().GetCamera()->Zoom),
            (float)window.GetWidth() / (float)window.GetHeight(),
            0.1f, VB::inst().GetGUI()->GetViewDistance());
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));

        // View
        glm::mat4 view = VB::inst().GetCamera()->GetViewMatrix();
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
                        
            VB::inst().GetVoxel()->RenderMesh(mesh, mesh.VAO);
        }

        glDisable(GL_DEPTH_TEST);
        crosshairShader.use();
        glBindVertexArray(VB::inst().GetGUI()->crosshairVAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);

        VB::inst().GetGUI()->UpdateImGui(window.GetGLFWwindow());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.SwapBuffers();
        window.PollEvents();
    }

    // Cleanup
    for (const Voxel::VoxelMesh& mesh : mesh_vec)
        VB::inst().GetVoxel()->FreeRenderMesh(mesh, const_cast<GLuint&>(mesh.VBO), const_cast<GLuint&>(mesh.EBO), const_cast<GLuint&>(mesh.VAO));

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}