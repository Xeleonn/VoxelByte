#include <glm/glm.hpp>
#include <vector>
#include "voxel.h"

Voxel::Voxel()
{
    voxelsGenerated = 0;
    float vertices[] = {
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
}

// World space positions of our cubes
std::vector<glm::vec3> cubePositions = {
    glm::vec3(-22.5f, -16.0f, -30.0f),   glm::vec3(-17.5f, -16.0f, -30.0f),   glm::vec3(-12.5f, -16.0f, -30.0f),   glm::vec3(-7.5f, -16.0f, -30.0f),    glm::vec3(-2.5f, -16.0f, -30.0f),    glm::vec3( 2.5f, -16.0f, -30.0f),    glm::vec3( 7.5f, -16.0f, -30.0f),    glm::vec3( 12.5f, -16.0f, -30.0f),   glm::vec3( 17.5f, -16.0f, -30.0f),   glm::vec3( 22.5f, -16.0f, -30.0f),   glm::vec3(-22.5f, 0.0f, -30.0f),     glm::vec3(-17.5f, 0.0f, -30.0f),   glm::vec3(-12.5f, 0.0f, -30.0f),     glm::vec3(-7.5f, 0.0f, -30.0f),      glm::vec3(-2.5f, 0.0f, -30.0f),      glm::vec3( 2.5f, 0.0f, -30.0f),    
    glm::vec3( 7.5f, 0.0f, -30.0f),      glm::vec3( 12.5f, 0.0f, -30.0f),     glm::vec3( 17.5f, 0.0f, -30.0f),     glm::vec3( 22.5f, 0.0f, -30.0f),   
    glm::vec3(-22.5f, 16.0f, -30.0f),    glm::vec3(-17.5f, 16.0f, -30.0f),    glm::vec3(-12.5f, 16.0f, -30.0f),    glm::vec3(-7.5f, 16.0f, -30.0f),    glm::vec3(-2.5f, 16.0f, -30.0f),     glm::vec3( 2.5f, 16.0f, -30.0f),     glm::vec3( 7.5f, 16.0f, -30.0f),     glm::vec3( 12.5f, 16.0f, -30.0f),   glm::vec3( 17.5f, 16.0f, -30.0f),    glm::vec3( 22.5f, 16.0f, -30.0f),    glm::vec3(-22.5f, -16.0f, -18.0f),   glm::vec3(-17.5f, -16.0f, -18.0f),   glm::vec3(-12.5f, -16.0f, -18.0f),   glm::vec3(-7.5f, -16.0f, -18.0f),    glm::vec3(-2.5f, -16.0f, -18.0f),    glm::vec3( 2.5f, -16.0f, -18.0f),    glm::vec3( 7.5f, -16.0f, -18.0f),    glm::vec3( 12.5f, -16.0f, -18.0f),   glm::vec3( 17.5f, -16.0f, -18.0f),   glm::vec3( 22.5f, -16.0f, -18.0f),   glm::vec3(-22.5f, 0.0f, -18.0f),     glm::vec3(-17.5f, 0.0f, -18.0f),     glm::vec3(-12.5f, 0.0f, -18.0f),     glm::vec3(-7.5f, 0.0f, -18.0f),    
    glm::vec3(-2.5f, 0.0f, -18.0f),      glm::vec3( 2.5f, 0.0f, -18.0f),      glm::vec3( 7.5f, 0.0f, -18.0f),      glm::vec3( 12.5f, 0.0f, -18.0f),    
    glm::vec3( 17.5f, 0.0f, -18.0f),     glm::vec3( 22.5f, 0.0f, -18.0f),     glm::vec3(-22.5f, 16.0f, -18.0f),    glm::vec3(-17.5f, 16.0f, -18.0f),    glm::vec3(-12.5f, 16.0f, -18.0f),    glm::vec3(-7.5f, 16.0f, -18.0f),     glm::vec3(-2.5f, 16.0f, -18.0f),     glm::vec3( 2.5f, 16.0f, -18.0f),    glm::vec3( 7.5f, 16.0f, -18.0f),     glm::vec3( 12.5f, 16.0f, -18.0f),    glm::vec3( 17.5f, 16.0f, -18.0f),    glm::vec3( 22.5f, 16.0f, -18.0f),   glm::vec3(-22.5f, -16.0f, -6.0f),    glm::vec3(-17.5f, -16.0f, -6.0f),    glm::vec3(-12.5f, -16.0f, -6.0f),    glm::vec3(-7.5f, -16.0f, -6.0f),    glm::vec3(-2.5f, -16.0f, -6.0f),     glm::vec3( 2.5f, -16.0f, -6.0f),     glm::vec3( 7.5f, -16.0f, -6.0f),     glm::vec3( 12.5f, -16.0f, -6.0f),    glm::vec3( 17.5f, -16.0f, -6.0f),    glm::vec3( 22.5f, -16.0f, -6.0f),    glm::vec3(-22.5f, 0.0f, -6.0f),      glm::vec3(-17.5f, 0.0f, -6.0f),    glm::vec3(-12.5f, 0.0f, -6.0f),      glm::vec3(-7.5f, 0.0f, -6.0f),       glm::vec3(-2.5f, 0.0f, -6.0f),       glm::vec3( 2.5f, 0.0f, -6.0f),    
    glm::vec3( 7.5f, 0.0f, -6.0f),       glm::vec3( 12.5f, 0.0f, -6.0f),      glm::vec3( 17.5f, 0.0f, -6.0f),      glm::vec3( 22.5f, 0.0f, -6.0f),   
    glm::vec3(-22.5f, 16.0f, -6.0f),     glm::vec3(-17.5f, 16.0f, -6.0f),     glm::vec3(-12.5f, 16.0f, -6.0f),     glm::vec3(-7.5f, 16.0f, -6.0f),    
    glm::vec3(-2.5f, 16.0f, -6.0f),      glm::vec3( 2.5f, 16.0f, -6.0f),      glm::vec3( 7.5f, 16.0f, -6.0f),      glm::vec3( 12.5f, 16.0f, -6.0f),    
    glm::vec3( 17.5f, 16.0f, -6.0f),     glm::vec3( 22.5f, 16.0f, -6.0f)
};

void Voxel::AddVoxel(float x, float y, float z, Clock clock)
{
    static float last_input_time = 0.0f;

    float current_time = clock.GetTime();

    if (current_time - last_input_time > 0.1f)
    {
        glm::vec3 new_voxel_position = glm::vec3(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));

        // Check if the vector is empty OR if the new voxel is different from the last one.
        if (::cubePositions.empty() || ::cubePositions.back() != new_voxel_position)
        {
            ::cubePositions.push_back(new_voxel_position);
            voxelsGenerated += 1;
            last_input_time = current_time;
        }
    }
}

void Voxel::DrawVoxel(int voxels, Shader shader)
{
    for (unsigned int i = 0; i < ::cubePositions.size(); i++)
    {
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, ::cubePositions.at(i));
        //float angle = 20.0f * i;
        //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
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