#include <glm/glm.hpp>
#include <vector>
#include "voxel.h"

Voxel::Voxel()
{
    voxelsGenerated = 0;
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
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, // back-right
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // front-right
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // front-right
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
}

// World space positions of our cubes
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
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

float Voxel::GetVertices()
{
    return vertices;
}

int Voxel::GetVoxelsGenerated()
{
    return voxelsGenerated;
}