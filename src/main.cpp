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
#include "camera.h"
#include "voxel.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void updateImGui();

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
bool wireframeMode = false;

// Voxel color values
float v_red = 1.0f;
float v_green = 1.0f;
float v_blue = 1.0f;

// Camera
Camera camera(glm::vec3(CHUNK_SIZE / 2, CHUNK_SIZE + 10.0f, CHUNK_SIZE * 1.5f + 5.0f)); // Position camera to view the chunk
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float viewDistance = 500.0f;

// Lighting
glm::vec3 lightPos(CHUNK_SIZE / 2.0f, CHUNK_SIZE * 1.5f, CHUNK_SIZE / 2.0f + 5.0f); // Position light relative to chunk

// Initialize clock
Clock game_clock;

// Initialize voxel system
Voxel voxel;

int main() {
    // GLFW: Initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "VoxelByte", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // GLAD: Load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float light_cube_vertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    const char *vertexLightingShaderSrc =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
    "}\n";

    const char *fragmentLightingShaderSrc =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 viewPos;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 objectColor;\n"
    "void main() {\n"
    "    float ambientStrength = 0.2;\n"
    "    vec3 ambient = ambientStrength * lightColor;\n"
    "    vec3 norm = normalize(Normal);\n"
    "    vec3 lightDir = normalize(lightPos - FragPos);\n"
    "    float diff = max(dot(norm, lightDir), 0.0);\n"
    "    vec3 diffuse = diff * lightColor;\n"
    "    float specularStrength = 0.5;\n"
    "    vec3 viewDir = normalize(viewPos - FragPos);\n"
    "    vec3 reflectDir = reflect(-lightDir, norm);\n"
    "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
    "    vec3 specular = specularStrength * spec * lightColor;\n"
    "    vec3 result = (ambient + diffuse + specular) * objectColor;\n"
    "    FragColor = vec4(result, 1.0);\n"
    "}\n";

    const char *vertexLightCubeShaderSrc =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\n";

    const char *fragmentLightCubeShaderSrc =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0); // White light source\n"
    "}\n";

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE); // Optional: enable face culling if confident in winding order
    // glCullFace(GL_BACK);

    Shader lightingShader(vertexLightingShaderSrc, fragmentLightingShaderSrc);
    Shader lightCubeShader(vertexLightCubeShaderSrc, fragmentLightCubeShaderSrc);

    // Setup for the light cube (lamp) VAO
    unsigned int lightCubeVBO, lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lightCubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(light_cube_vertices), light_cube_vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);
    // Position attribute for light cube
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    io.IniFilename = nullptr;

    // Test chunk generation and mesh setup
    Voxel::Chunk test_chunk = voxel.GenerateTestChunk();
    Voxel::OpenGLMesh test_mesh = voxel.GenerateChunkMesh(test_chunk);

    GLuint chunkVBO = 0, chunkEBO = 0, chunkVAO = 0; // Initialize to 0
    if (test_mesh.vertex_data.size() > 0 && test_mesh.indices.size() > 0) {
        voxel.SetupRenderMesh(test_mesh, chunkVBO, chunkEBO, chunkVAO);
    } else {
        std::cout << "Warning: Test chunk mesh is empty. Nothing to render for the chunk." << std::endl;
    }
    

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        game_clock.Update();
        processInput(window);

        glClearColor(0.7f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // View/projection transformations (common to both shaders)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, viewDistance);
        glm::mat4 view = camera.GetViewMatrix();

        // Render the chunk
        if (chunkVAO != 0) { // Only render if VAO was successfully created
            lightingShader.use();
            lightingShader.setVec3("objectColor", v_red, v_green, v_blue);
            lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            lightingShader.setVec3("lightPos", lightPos);
            lightingShader.setVec3("viewPos", camera.Position);
            lightingShader.setMat4("projection", projection);
            lightingShader.setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f); // Chunk is at origin (0,0,0) in its local space
            // If chunk.origin_x,y,z are to be used for world position, translate here:
            // model = glm::translate(model, glm::vec3(test_chunk.origin_x, test_chunk.origin_y, test_chunk.origin_z));
            lightingShader.setMat4("model", model);
            
            voxel.RenderMesh(test_mesh, chunkVAO);
        }

        // Render the light cube (lamp)
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glm::mat4 modelLamp = glm::mat4(1.0f);
        modelLamp = glm::translate(modelLamp, lightPos);
        modelLamp = glm::scale(modelLamp, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", modelLamp);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

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
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &lightCubeVBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    // Control cursor visibility with Right Mouse Button
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
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
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    // Only process mouse if cursor is disabled (i.e., not in ImGui interaction mode)
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

        camera.ProcessMouseMovement(xoffset, yoffset);
    } else { // If cursor is normal, reset firstMouse so movement isn't jerky when re-disabling
        firstMouse = true;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

void updateImGui() {
    ImGui::SetNextWindowSize(ImVec2(250, 300));
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::Begin("VoxelByte Info", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
    ImGui::Text("Time: %.2f s", game_clock.GetTime());
    ImGui::Text("Delta Time: %.4f ms", game_clock.GetDeltaTime() * 1000.0f); // ms
    ImGui::Text("%.1f FPS", game_clock.GetFPS());

    ImGui::Separator();
    ImGui::Text("Camera Position:");
    ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", camera.Position.x, camera.Position.y, camera.Position.z);
    ImGui::Text("Yaw: %.1f, Pitch: %.1f", camera.Yaw, camera.Pitch);

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Settings")) {
        ImGui::Checkbox("Wireframe Meshes", &wireframeMode);
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        ImGui::Separator();
        ImGui::Text("Chunk Color (Lighting):");
        ImGui::ColorEdit3("Color", &v_red);
        ImGui::Spacing();
    }
    ImGui::End();
}