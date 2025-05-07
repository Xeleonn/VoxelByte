#include <glm/glm.hpp>
#include <vector>
#include "voxel.h"

Voxel::Voxel()
{
    voxelsGenerated = 0;
    float vertices[] = {
        // positions          // colors
        // Front face (Red)
        -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // top-right
         1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // top-left
        -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // Back face (Green)
        -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        // Left face (Blue)
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-front
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-back
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f, // bottom-back
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f, // bottom-back
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, // bottom-front
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-front
        // Right face (Yellow)
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f, // top-front
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f, // top-back
         1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, // bottom-back
         1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, // bottom-back
         1.0f, -1.0f,  1.0f,  1.0f, 1.0f, 0.0f, // bottom-front
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f, // top-front
        // Bottom face (Cyan)
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f, // back-left
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f, // back-right
         1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // front-right
         1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // front-right
        -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // front-left
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f, // back-left
        // Top face (Magenta)
        -1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 1.0f, // back-left
         1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 1.0f, // back-right
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f, // front-right
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f, // front-right
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f, // front-left
        -1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 1.0f  // back-left
    };
}

// World space positions of our cubes
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f, 0.0f, 0.0f),     glm::vec3(-22.5f, -16.0f, -30.0f),   glm::vec3(-17.5f, -16.0f, -30.0f),   glm::vec3(-12.5f, -16.0f, -30.0f),
    glm::vec3(-7.5f, -16.0f, -30.0f),    glm::vec3(-2.5f, -16.0f, -30.0f),    glm::vec3( 2.5f, -16.0f, -30.0f),    glm::vec3( 7.5f, -16.0f, -30.0f),
    glm::vec3( 12.5f, -16.0f, -30.0f),   glm::vec3( 17.5f, -16.0f, -30.0f),   glm::vec3( 22.5f, -16.0f, -30.0f),   glm::vec3(-22.5f, 0.0f, -30.0f),
    glm::vec3(-17.5f, 0.0f, -30.0f),   glm::vec3(-12.5f, 0.0f, -30.0f),   glm::vec3(-7.5f, 0.0f, -30.0f),    glm::vec3(-2.5f, 0.0f, -30.0f),
    glm::vec3( 2.5f, 0.0f, -30.0f),    glm::vec3( 7.5f, 0.0f, -30.0f),    glm::vec3( 12.5f, 0.0f, -30.0f),   glm::vec3( 17.5f, 0.0f, -30.0f),
    glm::vec3( 22.5f, 0.0f, -30.0f),   glm::vec3(-22.5f, 16.0f, -30.0f),   glm::vec3(-17.5f, 16.0f, -30.0f),   glm::vec3(-12.5f, 16.0f, -30.0f),
    glm::vec3(-7.5f, 16.0f, -30.0f),    glm::vec3(-2.5f, 16.0f, -30.0f),    glm::vec3( 2.5f, 16.0f, -30.0f),    glm::vec3( 7.5f, 16.0f, -30.0f),
    glm::vec3( 12.5f, 16.0f, -30.0f),   glm::vec3( 17.5f, 16.0f, -30.0f),   glm::vec3( 22.5f, 16.0f, -30.0f),   glm::vec3(-22.5f, -16.0f, -18.0f),
    glm::vec3(-17.5f, -16.0f, -18.0f),   glm::vec3(-12.5f, -16.0f, -18.0f),   glm::vec3(-7.5f, -16.0f, -18.0f),    glm::vec3(-2.5f, -16.0f, -18.0f),
    glm::vec3( 2.5f, -16.0f, -18.0f),    glm::vec3( 7.5f, -16.0f, -18.0f),    glm::vec3( 12.5f, -16.0f, -18.0f),   glm::vec3( 17.5f, -16.0f, -18.0f),
    glm::vec3( 22.5f, -16.0f, -18.0f),   glm::vec3(-22.5f, 0.0f, -18.0f),   glm::vec3(-17.5f, 0.0f, -18.0f),    glm::vec3(-12.5f, 0.0f, -18.0f),
    glm::vec3(-7.5f, 0.0f, -18.0f),    glm::vec3(-2.5f, 0.0f, -18.0f),    glm::vec3( 2.5f, 0.0f, -18.0f),    glm::vec3( 7.5f, 0.0f, -18.0f),
    glm::vec3( 12.5f, 0.0f, -18.0f),    glm::vec3( 17.5f, 0.0f, -18.0f),    glm::vec3( 22.5f, 0.0f, -18.0f),   glm::vec3(-22.5f, 16.0f, -18.0f),
    glm::vec3(-17.5f, 16.0f, -18.0f),    glm::vec3(-12.5f, 16.0f, -18.0f),    glm::vec3(-7.5f, 16.0f, -18.0f),    glm::vec3(-2.5f, 16.0f, -18.0f),
    glm::vec3( 2.5f, 16.0f, -18.0f),    glm::vec3( 7.5f, 16.0f, -18.0f),    glm::vec3( 12.5f, 16.0f, -18.0f),   glm::vec3( 17.5f, 16.0f, -18.0f),
    glm::vec3( 22.5f, 16.0f, -18.0f),   glm::vec3(-22.5f, -16.0f, -6.0f),   glm::vec3(-17.5f, -16.0f, -6.0f),    glm::vec3(-12.5f, -16.0f, -6.0f),
    glm::vec3(-7.5f, -16.0f, -6.0f),    glm::vec3(-2.5f, -16.0f, -6.0f),    glm::vec3( 2.5f, -16.0f, -6.0f),    glm::vec3( 7.5f, -16.0f, -6.0f),
    glm::vec3( 12.5f, -16.0f, -6.0f),    glm::vec3( 17.5f, -16.0f, -6.0f),    glm::vec3( 22.5f, -16.0f, -6.0f),   glm::vec3(-22.5f, 0.0f, -6.0f),
    glm::vec3(-17.5f, 0.0f, -6.0f),    glm::vec3(-12.5f, 0.0f, -6.0f),    glm::vec3(-7.5f, 0.0f, -6.0f),    glm::vec3(-2.5f, 0.0f, -6.0f),
    glm::vec3( 2.5f, 0.0f, -6.0f),    glm::vec3( 7.5f, 0.0f, -6.0f),    glm::vec3( 12.5f, 0.0f, -6.0f),    glm::vec3( 17.5f, 0.0f, -6.0f),
    glm::vec3( 22.5f, 0.0f, -6.0f),   glm::vec3(-22.5f, 16.0f, -6.0f),   glm::vec3(-17.5f, 16.0f, -6.0f),    glm::vec3(-12.5f, 16.0f, -6.0f),
    glm::vec3(-7.5f, 16.0f, -6.0f),    glm::vec3(-2.5f, 16.0f, -6.0f),    glm::vec3( 2.5f, 16.0f, -6.0f),    glm::vec3( 7.5f, 16.0f, -6.0f),
    glm::vec3( 12.5f, 16.0f, -6.0f),    glm::vec3( 17.5f, 16.0f, -6.0f),    glm::vec3( 22.5f, 16.0f, -6.0f)
};

void Voxel::DrawVoxel(int voxels, Shader ourShader)
{
    for (unsigned int i = 0; i < voxels; i++)
    {
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, ::cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        if (voxelsGenerated < voxels)
            voxelsGenerated += 1;
    }
}

int Voxel::GetVoxelsGenerated()
{
    return voxelsGenerated;
}