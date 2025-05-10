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

Voxel::VoxelColor& Voxel::Chunk::GetColor(Voxel::VoxelData voxel)
{
    int id = voxel.voxel_id;
    float r = voxel_colors[id][0];
    float g = voxel_colors[id][1];
    float b = voxel_colors[id][2];
    Voxel::VoxelColor color{r, g, b}; 
    return color;
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
                VoxelColor vc;
                vd = {0, VoxelProperties_Solid, 0};
                vc = {voxel_colors[vd.voxel_id][0], voxel_colors[vd.voxel_id][1], voxel_colors[vd.voxel_id][2]};
                //else vd = {0, VoxelProperties_Null, 0};
                new_chunk.GetVoxel(x, y, z) = vd;
                new_chunk.GetColor(vd) = vc;
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
                    float r;
                    float g;
                    float b;
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
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
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

static const float voxel_colors[256][3] = {
    // Colors 0-15: ANSI/Xterm standard colors
    {0.00000f, 0.00000f, 0.00000f}, // 0: Black
    {0.80392f, 0.00000f, 0.00000f}, // 1: Red (Maroon Variant for xterm)
    {0.00000f, 0.80392f, 0.00000f}, // 2: Green (Green Variant for xterm)
    {0.80392f, 0.80392f, 0.00000f}, // 3: Yellow (Olive Variant for xterm)
    {0.00000f, 0.00000f, 0.93333f}, // 4: Blue (Navy Variant for xterm)
    {0.80392f, 0.00000f, 0.80392f}, // 5: Magenta (Purple Variant for xterm)
    {0.00000f, 0.80392f, 0.80392f}, // 6: Cyan (Teal Variant for xterm)
    {0.89804f, 0.89804f, 0.89804f}, // 7: White (Light Gray for xterm)
    {0.49804f, 0.49804f, 0.49804f}, // 8: Bright Black (Gray for xterm)
    {1.00000f, 0.00000f, 0.00000f}, // 9: Bright Red
    {0.00000f, 1.00000f, 0.00000f}, // 10: Bright Green
    {1.00000f, 1.00000f, 0.00000f}, // 11: Bright Yellow
    {0.36078f, 0.36078f, 1.00000f}, // 12: Bright Blue (Specific xterm Bright Blue)
    {1.00000f, 0.00000f, 1.00000f}, // 13: Bright Magenta
    {0.00000f, 1.00000f, 1.00000f}, // 14: Bright Cyan
    {1.00000f, 1.00000f, 1.00000f}, // 15: Bright White

    // Colors 16-231: 6x6x6 color cube
    // R levels: 0, 95, 135, 175, 215, 255 -> 0.0f, 0.37255f, 0.52941f, 0.68627f, 0.84314f, 1.0f
    // G levels: 0, 95, 135, 175, 215, 255
    // B levels: 0, 95, 135, 175, 215, 255

    // R=0 (0.0f)
    {0.00000f, 0.00000f, 0.00000f}, {0.00000f, 0.00000f, 0.37255f}, {0.00000f, 0.00000f, 0.52941f}, {0.00000f, 0.00000f, 0.68627f}, {0.00000f, 0.00000f, 0.84314f}, {0.00000f, 0.00000f, 1.00000f},
    {0.00000f, 0.37255f, 0.00000f}, {0.00000f, 0.37255f, 0.37255f}, {0.00000f, 0.37255f, 0.52941f}, {0.00000f, 0.37255f, 0.68627f}, {0.00000f, 0.37255f, 0.84314f}, {0.00000f, 0.37255f, 1.00000f},
    {0.00000f, 0.52941f, 0.00000f}, {0.00000f, 0.52941f, 0.37255f}, {0.00000f, 0.52941f, 0.52941f}, {0.00000f, 0.52941f, 0.68627f}, {0.00000f, 0.52941f, 0.84314f}, {0.00000f, 0.52941f, 1.00000f},
    {0.00000f, 0.68627f, 0.00000f}, {0.00000f, 0.68627f, 0.37255f}, {0.00000f, 0.68627f, 0.52941f}, {0.00000f, 0.68627f, 0.68627f}, {0.00000f, 0.68627f, 0.84314f}, {0.00000f, 0.68627f, 1.00000f},
    {0.00000f, 0.84314f, 0.00000f}, {0.00000f, 0.84314f, 0.37255f}, {0.00000f, 0.84314f, 0.52941f}, {0.00000f, 0.84314f, 0.68627f}, {0.00000f, 0.84314f, 0.84314f}, {0.00000f, 0.84314f, 1.00000f},
    {0.00000f, 1.00000f, 0.00000f}, {0.00000f, 1.00000f, 0.37255f}, {0.00000f, 1.00000f, 0.52941f}, {0.00000f, 1.00000f, 0.68627f}, {0.00000f, 1.00000f, 0.84314f}, {0.00000f, 1.00000f, 1.00000f},

    // R=1 (0.37255f)
    {0.37255f, 0.00000f, 0.00000f}, {0.37255f, 0.00000f, 0.37255f}, {0.37255f, 0.00000f, 0.52941f}, {0.37255f, 0.00000f, 0.68627f}, {0.37255f, 0.00000f, 0.84314f}, {0.37255f, 0.00000f, 1.00000f},
    {0.37255f, 0.37255f, 0.00000f}, {0.37255f, 0.37255f, 0.37255f}, {0.37255f, 0.37255f, 0.52941f}, {0.37255f, 0.37255f, 0.68627f}, {0.37255f, 0.37255f, 0.84314f}, {0.37255f, 0.37255f, 1.00000f},
    {0.37255f, 0.52941f, 0.00000f}, {0.37255f, 0.52941f, 0.37255f}, {0.37255f, 0.52941f, 0.52941f}, {0.37255f, 0.52941f, 0.68627f}, {0.37255f, 0.52941f, 0.84314f}, {0.37255f, 0.52941f, 1.00000f},
    {0.37255f, 0.68627f, 0.00000f}, {0.37255f, 0.68627f, 0.37255f}, {0.37255f, 0.68627f, 0.52941f}, {0.37255f, 0.68627f, 0.68627f}, {0.37255f, 0.68627f, 0.84314f}, {0.37255f, 0.68627f, 1.00000f},
    {0.37255f, 0.84314f, 0.00000f}, {0.37255f, 0.84314f, 0.37255f}, {0.37255f, 0.84314f, 0.52941f}, {0.37255f, 0.84314f, 0.68627f}, {0.37255f, 0.84314f, 0.84314f}, {0.37255f, 0.84314f, 1.00000f},
    {0.37255f, 1.00000f, 0.00000f}, {0.37255f, 1.00000f, 0.37255f}, {0.37255f, 1.00000f, 0.52941f}, {0.37255f, 1.00000f, 0.68627f}, {0.37255f, 1.00000f, 0.84314f}, {0.37255f, 1.00000f, 1.00000f},

    // R=2 (0.52941f)
    {0.52941f, 0.00000f, 0.00000f}, {0.52941f, 0.00000f, 0.37255f}, {0.52941f, 0.00000f, 0.52941f}, {0.52941f, 0.00000f, 0.68627f}, {0.52941f, 0.00000f, 0.84314f}, {0.52941f, 0.00000f, 1.00000f},
    {0.52941f, 0.37255f, 0.00000f}, {0.52941f, 0.37255f, 0.37255f}, {0.52941f, 0.37255f, 0.52941f}, {0.52941f, 0.37255f, 0.68627f}, {0.52941f, 0.37255f, 0.84314f}, {0.52941f, 0.37255f, 1.00000f},
    {0.52941f, 0.52941f, 0.00000f}, {0.52941f, 0.52941f, 0.37255f}, {0.52941f, 0.52941f, 0.52941f}, {0.52941f, 0.52941f, 0.68627f}, {0.52941f, 0.52941f, 0.84314f}, {0.52941f, 0.52941f, 1.00000f},
    {0.52941f, 0.68627f, 0.00000f}, {0.52941f, 0.68627f, 0.37255f}, {0.52941f, 0.68627f, 0.52941f}, {0.52941f, 0.68627f, 0.68627f}, {0.52941f, 0.68627f, 0.84314f}, {0.52941f, 0.68627f, 1.00000f},
    {0.52941f, 0.84314f, 0.00000f}, {0.52941f, 0.84314f, 0.37255f}, {0.52941f, 0.84314f, 0.52941f}, {0.52941f, 0.84314f, 0.68627f}, {0.52941f, 0.84314f, 0.84314f}, {0.52941f, 0.84314f, 1.00000f},
    {0.52941f, 1.00000f, 0.00000f}, {0.52941f, 1.00000f, 0.37255f}, {0.52941f, 1.00000f, 0.52941f}, {0.52941f, 1.00000f, 0.68627f}, {0.52941f, 1.00000f, 0.84314f}, {0.52941f, 1.00000f, 1.00000f},

    // R=3 (0.68627f)
    {0.68627f, 0.00000f, 0.00000f}, {0.68627f, 0.00000f, 0.37255f}, {0.68627f, 0.00000f, 0.52941f}, {0.68627f, 0.00000f, 0.68627f}, {0.68627f, 0.00000f, 0.84314f}, {0.68627f, 0.00000f, 1.00000f},
    {0.68627f, 0.37255f, 0.00000f}, {0.68627f, 0.37255f, 0.37255f}, {0.68627f, 0.37255f, 0.52941f}, {0.68627f, 0.37255f, 0.68627f}, {0.68627f, 0.37255f, 0.84314f}, {0.68627f, 0.37255f, 1.00000f},
    {0.68627f, 0.52941f, 0.00000f}, {0.68627f, 0.52941f, 0.37255f}, {0.68627f, 0.52941f, 0.52941f}, {0.68627f, 0.52941f, 0.68627f}, {0.68627f, 0.52941f, 0.84314f}, {0.68627f, 0.52941f, 1.00000f},
    {0.68627f, 0.68627f, 0.00000f}, {0.68627f, 0.68627f, 0.37255f}, {0.68627f, 0.68627f, 0.52941f}, {0.68627f, 0.68627f, 0.68627f}, {0.68627f, 0.68627f, 0.84314f}, {0.68627f, 0.68627f, 1.00000f},
    {0.68627f, 0.84314f, 0.00000f}, {0.68627f, 0.84314f, 0.37255f}, {0.68627f, 0.84314f, 0.52941f}, {0.68627f, 0.84314f, 0.68627f}, {0.68627f, 0.84314f, 0.84314f}, {0.68627f, 0.84314f, 1.00000f},
    {0.68627f, 1.00000f, 0.00000f}, {0.68627f, 1.00000f, 0.37255f}, {0.68627f, 1.00000f, 0.52941f}, {0.68627f, 1.00000f, 0.68627f}, {0.68627f, 1.00000f, 0.84314f}, {0.68627f, 1.00000f, 1.00000f},

    // R=4 (0.84314f)
    {0.84314f, 0.00000f, 0.00000f}, {0.84314f, 0.00000f, 0.37255f}, {0.84314f, 0.00000f, 0.52941f}, {0.84314f, 0.00000f, 0.68627f}, {0.84314f, 0.00000f, 0.84314f}, {0.84314f, 0.00000f, 1.00000f},
    {0.84314f, 0.37255f, 0.00000f}, {0.84314f, 0.37255f, 0.37255f}, {0.84314f, 0.37255f, 0.52941f}, {0.84314f, 0.37255f, 0.68627f}, {0.84314f, 0.37255f, 0.84314f}, {0.84314f, 0.37255f, 1.00000f},
    {0.84314f, 0.52941f, 0.00000f}, {0.84314f, 0.52941f, 0.37255f}, {0.84314f, 0.52941f, 0.52941f}, {0.84314f, 0.52941f, 0.68627f}, {0.84314f, 0.52941f, 0.84314f}, {0.84314f, 0.52941f, 1.00000f},
    {0.84314f, 0.68627f, 0.00000f}, {0.84314f, 0.68627f, 0.37255f}, {0.84314f, 0.68627f, 0.52941f}, {0.84314f, 0.68627f, 0.68627f}, {0.84314f, 0.68627f, 0.84314f}, {0.84314f, 0.68627f, 1.00000f},
    {0.84314f, 0.84314f, 0.00000f}, {0.84314f, 0.84314f, 0.37255f}, {0.84314f, 0.84314f, 0.52941f}, {0.84314f, 0.84314f, 0.68627f}, {0.84314f, 0.84314f, 0.84314f}, {0.84314f, 0.84314f, 1.00000f},
    {0.84314f, 1.00000f, 0.00000f}, {0.84314f, 1.00000f, 0.37255f}, {0.84314f, 1.00000f, 0.52941f}, {0.84314f, 1.00000f, 0.68627f}, {0.84314f, 1.00000f, 0.84314f}, {0.84314f, 1.00000f, 1.00000f},

    // R=5 (1.0f)
    {1.00000f, 0.00000f, 0.00000f}, {1.00000f, 0.00000f, 0.37255f}, {1.00000f, 0.00000f, 0.52941f}, {1.00000f, 0.00000f, 0.68627f}, {1.00000f, 0.00000f, 0.84314f}, {1.00000f, 0.00000f, 1.00000f},
    {1.00000f, 0.37255f, 0.00000f}, {1.00000f, 0.37255f, 0.37255f}, {1.00000f, 0.37255f, 0.52941f}, {1.00000f, 0.37255f, 0.68627f}, {1.00000f, 0.37255f, 0.84314f}, {1.00000f, 0.37255f, 1.00000f},
    {1.00000f, 0.52941f, 0.00000f}, {1.00000f, 0.52941f, 0.37255f}, {1.00000f, 0.52941f, 0.52941f}, {1.00000f, 0.52941f, 0.68627f}, {1.00000f, 0.52941f, 0.84314f}, {1.00000f, 0.52941f, 1.00000f},
    {1.00000f, 0.68627f, 0.00000f}, {1.00000f, 0.68627f, 0.37255f}, {1.00000f, 0.68627f, 0.52941f}, {1.00000f, 0.68627f, 0.68627f}, {1.00000f, 0.68627f, 0.84314f}, {1.00000f, 0.68627f, 1.00000f},
    {1.00000f, 0.84314f, 0.00000f}, {1.00000f, 0.84314f, 0.37255f}, {1.00000f, 0.84314f, 0.52941f}, {1.00000f, 0.84314f, 0.68627f}, {1.00000f, 0.84314f, 0.84314f}, {1.00000f, 0.84314f, 1.00000f},
    {1.00000f, 1.00000f, 0.00000f}, {1.00000f, 1.00000f, 0.37255f}, {1.00000f, 1.00000f, 0.52941f}, {1.00000f, 1.00000f, 0.68627f}, {1.00000f, 1.00000f, 0.84314f}, {1.00000f, 1.00000f, 1.00000f}, // Color 231

    // Colors 232-255: Grayscale ramp (24 colors)
    // Gray levels: 8, 18, 28, ..., 238 -> 0.03137f, 0.07059f, ..., 0.93333f
    {0.03137f, 0.03137f, 0.03137f}, {0.07059f, 0.07059f, 0.07059f}, {0.10980f, 0.10980f, 0.10980f}, {0.14902f, 0.14902f, 0.14902f},
    {0.18824f, 0.18824f, 0.18824f}, {0.22745f, 0.22745f, 0.22745f}, {0.26667f, 0.26667f, 0.26667f}, {0.30588f, 0.30588f, 0.30588f},
    {0.34510f, 0.34510f, 0.34510f}, {0.38431f, 0.38431f, 0.38431f}, {0.42353f, 0.42353f, 0.42353f}, {0.46275f, 0.46275f, 0.46275f},
    {0.50196f, 0.50196f, 0.50196f}, {0.54118f, 0.54118f, 0.54118f}, {0.58039f, 0.58039f, 0.58039f}, {0.61961f, 0.61961f, 0.61961f},
    {0.65882f, 0.65882f, 0.65882f}, {0.69804f, 0.69804f, 0.69804f}, {0.73725f, 0.73725f, 0.73725f}, {0.77647f, 0.77647f, 0.77647f},
    {0.81569f, 0.81569f, 0.81569f}, {0.85490f, 0.85490f, 0.85490f}, {0.89412f, 0.89412f, 0.89412f}, {0.93333f, 0.93333f, 0.93333f}  // Color 255
};