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

Voxel::Chunk Voxel::GenerateTestChunk()
{
    Chunk new_chunk;
    new_chunk.origin_x = 5;
    new_chunk.origin_y = 2;
    new_chunk.origin_z = 14;

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                VoxelData vd;
                if (y % 3 == 0) vd = {7, 2, 15};
                else vd = {4, 0, 15};
                new_chunk.voxel_grid[x][y][z] = vd;
            }
        }
    }
    return new_chunk;
}

int Voxel::OpenGLMesh::AddVertex(float a, float b, float c)
{
    vertices.push_back(a);
    vertices.push_back(b);
    vertices.push_back(c);
    return (vertices.size() / 3) - 1;
}

void Voxel::OpenGLMesh::AddIndex(int v0, int v1, int v2)
{
    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(v2);
}

Voxel::OpenGLMesh Voxel::GenerateChunkMesh(Chunk chunk)
{
    OpenGLMesh chunk_mesh;

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            for (int z = 0; z < CHUNK_SIZE; z++)
            {
                if (chunk.voxel_grid[x][y][z].properties == 2)
                {
                    // add triangles to top
                    if ((y + 1  == CHUNK_SIZE) || (chunk.voxel_grid[x][y + 1][z].properties == 0))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z + 1);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z + 1);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to bottom
                    if ((y == 0) || (chunk.voxel_grid[x][y - 1][z].properties == 0))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z + 1);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y, (float)z + 1);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to right
                    if ((x + 1 == CHUNK_SIZE) || (chunk.voxel_grid[x + 1][y][z].properties == 0))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z + 1);
                        int v3 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z + 1);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to left
                    if ((x == 0) || (chunk.voxel_grid[x - 1][y][z].properties == 0))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z);
                        int v1 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z);
                        int v2 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z + 1);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y, (float)z + 1);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to back
                    if ((z + 1 == CHUNK_SIZE) || (chunk.voxel_grid[x][y][z + 1].properties == 0))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z + 1);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z + 1);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z + 1);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z + 1);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to front
                    if ((z == 0) || (chunk.voxel_grid[x][y][z - 1].properties == 0))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                }
            }
        }
    }
    return chunk_mesh;
}

/*
void Voxel::RenderMesh(OpenGLMesh mesh)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
*/

void  Voxel::SetupRenderMesh(OpenGLMesh mesh, GLuint VBO, GLuint EBO, GLuint VAO)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void  Voxel::RenderMesh(OpenGLMesh mesh, GLuint VAO)
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
}

void  Voxel::FreeRenderMesh(OpenGLMesh mesh, GLuint VBO, GLuint EBO, GLuint VAO)
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

int Voxel::GetVoxelsGenerated()
{
    return voxelsGenerated;
}