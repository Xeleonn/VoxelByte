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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <iostream>
#include <unordered_map>

const char* TEXTURE_PATH = "../textures/texture_atlas.png";
GLuint textureID;

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

    // Texture loading
    {
        int width, height, channels;
        char* data = (char*)stbi_load(TEXTURE_PATH, &width, &height, &channels, 4);
        if (!data)
        {
            VB::inst().GetLogger()->PrintErr("Failed to load texture");
            return false;
        }
        VB::inst().GetLogger()->Print(std::to_string(width) + " " + std::to_string(height) + " " + std::to_string(channels));

        glGenTextures(1, &textureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // This setting only matters when using the GLSL texture() function
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
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
        VoxelShader->setInt("textureAtlas", 0);
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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

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