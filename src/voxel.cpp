#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "voxel.h"
#include "shader.h"
#include "clock.h"

Voxel::Voxel()
{

}

Voxel::Chunk::Chunk()
{
    voxel_array.resize(Voxel::CHUNK_SIZE * Voxel::CHUNK_SIZE * Voxel::CHUNK_SIZE);
}

Voxel::VoxelData& Voxel::Chunk::GetVoxel(int pos_x, int pos_y, int pos_z)
{
    return voxel_array[pos_z * CHUNK_SIZE * CHUNK_SIZE + pos_y * CHUNK_SIZE + pos_x];
}

Voxel::Chunk Voxel::GenerateTestChunk()
{

    Chunk new_chunk;
    new_chunk.origin_x = 0;
    new_chunk.origin_y = 0;
    new_chunk.origin_z = 0;

    for (int x = 0; x < Voxel::CHUNK_SIZE; x++)
    {
        for (int y = 0; y < Voxel::CHUNK_SIZE; y++)
        {
            for (int z = 0; z < Voxel::CHUNK_SIZE; z++)
            {
                VoxelData vd;
                vd = {0, VoxelProperties_Solid, 0};
                //else vd = {0, VoxelProperties_Null, 0};
                new_chunk.GetVoxel(x, y, z) = vd;
            }
        }
    }

    return new_chunk;
}

int Voxel::OpenGLMesh::AddVertex(float x, float y, float z)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    return (vertices.size() / 3) - 1;
}

void Voxel::OpenGLMesh::AddIndex(int v0, int v1, int v2) {
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
                if (chunk.GetVoxel(x, y, z).properties & VoxelProperties::VoxelProperties_Solid)
                {
                    // add triangles to top
                    if ((y + 1  == CHUNK_SIZE) || ((chunk.GetVoxel(x, y + 1, z).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z + 1);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z + 1);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to bottom
                    if ((y == 0) || ((chunk.GetVoxel(x, y - 1, z).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z + 1);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y, (float)z + 1);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to right
                    if ((x + 1 == CHUNK_SIZE) || ((chunk.GetVoxel(x + 1, y, z).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z + 1);
                        int v3 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z + 1);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to left
                    if ((x == 0) || ((chunk.GetVoxel(x - 1, y, z).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z);
                        int v1 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z);
                        int v2 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z + 1);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y, (float)z + 1);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to back
                    if ((z + 1 == CHUNK_SIZE) || ((chunk.GetVoxel(x, y, z + 1).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z + 1);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z + 1);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z + 1);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z + 1);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to front
                    if ((z == 0) || ((chunk.GetVoxel(x, y, z - 1).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
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

void Voxel::SetupRenderMesh(OpenGLMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO)
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

void Voxel::RenderMesh(OpenGLMesh mesh, GLuint VAO)
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Voxel::FreeRenderMesh(OpenGLMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO)
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    VAO = 0;
    VBO = 0;
    EBO = 0;
}