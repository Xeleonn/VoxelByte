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

    std::shared_ptr<Shader> VoxelShader = std::make_shared<Shader>("../shaders/voxel_vert.glsl", "../shaders/voxel_frag.glsl");
    Shader crosshairShader("../shaders/crosshair_vert.glsl", "../shaders/crosshair_frag.glsl");

    // Setup crosshair
    VB::inst().GetGUI()->SetupCrosshairMesh();

    VB::inst().GetMultiChunkSystem()->update();

    VB::inst().GetVoxel()->SetShader(VoxelShader);
    for (const auto& id_chunk : VB::inst().GetMultiChunkSystem()->get_chunk_map())
    {
        VoxelRenderer::VoxelMesh curr_mesh = VB::inst().GetVoxel()->GenerateChunkMesh(*(id_chunk.second));
        VB::inst().GetVoxel()->BufferVoxelMesh(id_chunk.first, curr_mesh);
    }
    // Enable OpenGL functionality
    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!window.ShouldClose()) {
        VB::inst().GetClock()->Update();
        VB::inst().GetInput()->ProcessInput(window.GetGLFWwindow());

        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Projection
        VoxelShader->use();
        glm::mat4 projection = glm::perspective(glm::radians(VB::inst().GetCamera()->Zoom),
            (float)window.GetWidth() / (float)window.GetHeight(),
            0.1f, VB::inst().GetGUI()->GetViewDistance());
        glUniformMatrix4fv(glGetUniformLocation(VoxelShader->ID, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));

        // View
        glm::mat4 view = VB::inst().GetCamera()->GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(VoxelShader->ID, "view"),
            1, GL_FALSE, glm::value_ptr(view));

        // Model
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(VoxelShader->ID, "model"),
            1, GL_FALSE, glm::value_ptr(model));

        VB::inst().GetVoxel()->RenderAllMeshes();

        crosshairShader.use();
        glBindVertexArray(VB::inst().GetGUI()->crosshairVAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        VB::inst().GetGUI()->UpdateImGui(window.GetGLFWwindow());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.SwapBuffers();
        window.PollEvents();
    }

    // Cleanup
    VB::inst().GetVoxel()->FreeRenderMeshes();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
