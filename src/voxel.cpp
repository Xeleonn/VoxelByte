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

// Change return type from Voxel::VoxelColor& to Voxel::VoxelColor
Voxel::VoxelColor Voxel::Chunk::GetColor(Voxel::VoxelData voxel)
{
    int id = voxel.voxel_id;
    Voxel::VoxelColor color_obj; // Renamed for clarity, still local
    color_obj = {voxel_colors[id][0], voxel_colors[id][1], voxel_colors[id][2]};
    return color_obj; // Return by value (a copy is made)
}

int Voxel::ColorHeight(int y, int chunk_size)
{
    if (y == chunk_size - 1)
        return 105; // Green
    if (y >= chunk_size - 30 && y < chunk_size - 1)
        return 20;
    if (y <= chunk_size - 31)
        return 220;
    else {
        return 255;
    }
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
                vd = {static_cast<unsigned char>(ColorHeight(y, CHUNK_SIZE)), VoxelProperties_Solid, 0};
                //else vd = {0, VoxelProperties_Null, 0};
                new_chunk.GetVoxel(x, y, z) = vd;
                float r = voxel_colors[vd.voxel_id][0];
                float g = voxel_colors[vd.voxel_id][1];
                float b = voxel_colors[vd.voxel_id][2];
            }
        }
    }

    return new_chunk;
}

int Voxel::OpenGLMesh::AddVertex(float x, float y, float z, float r, float g, float b)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(r);
    vertices.push_back(g);
    vertices.push_back(b);
    return (vertices.size() / 6) - 1;
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
                    VoxelData current_voxel_data = chunk.GetVoxel(x, y, z);
                    VoxelColor voxel_color = chunk.GetColor(current_voxel_data);

                    // add triangles to top
                    if ((y + 1  == CHUNK_SIZE) || ((chunk.GetVoxel(x, y + 1, z).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to bottom
                    if ((y == 0) || ((chunk.GetVoxel(x, y - 1, z).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to right
                    if ((x + 1 == CHUNK_SIZE) || ((chunk.GetVoxel(x + 1, y, z).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v3 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to left
                    if ((x == 0) || ((chunk.GetVoxel(x - 1, y, z).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v1 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v2 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to back
                    if ((z + 1 == CHUNK_SIZE) || ((chunk.GetVoxel(x, y, z + 1).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z + 1, voxel_color.r, voxel_color.g, voxel_color.b);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }
                    // add triangles to front
                    if ((z == 0) || ((chunk.GetVoxel(x, y, z - 1).properties & VoxelProperties::VoxelProperties_Solid) == VoxelProperties::VoxelProperties_Null))
                    {
                        int v0 = chunk_mesh.AddVertex((float)x, (float)y, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v1 = chunk_mesh.AddVertex((float)x + 1, (float)y, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v2 = chunk_mesh.AddVertex((float)x + 1, (float)y + 1, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
                        int v3 = chunk_mesh.AddVertex((float)x, (float)y + 1, (float)z, voxel_color.r, voxel_color.g, voxel_color.b);
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

const float Voxel::voxel_colors[256][3] = {
    // --- Rainbow Colors (Indices 0-191) ---
    // Segment: Red to Orange
    {1.00000f, 0.00000f, 0.00000f}, // 0: Red
    {0.96875f, 0.01562f, 0.00000f}, // 1
    {0.93750f, 0.03125f, 0.00000f}, // 2
    {0.90625f, 0.04688f, 0.00000f}, // 3
    {0.87500f, 0.06250f, 0.00000f}, // 4
    {0.84375f, 0.07812f, 0.00000f}, // 5
    {0.81250f, 0.09375f, 0.00000f}, // 6
    {0.78125f, 0.10938f, 0.00000f}, // 7
    {0.75000f, 0.12500f, 0.00000f}, // 8
    {0.71875f, 0.14062f, 0.00000f}, // 9
    {0.68750f, 0.15625f, 0.00000f}, // 10
    {0.65625f, 0.17188f, 0.00000f}, // 11
    {0.62500f, 0.18750f, 0.00000f}, // 12
    {0.59375f, 0.20312f, 0.00000f}, // 13
    {0.56250f, 0.21875f, 0.00000f}, // 14
    {0.53125f, 0.23438f, 0.00000f}, // 15
    {0.50000f, 0.25000f, 0.00000f}, // 16
    {0.46875f, 0.26562f, 0.00000f}, // 17
    {0.43750f, 0.28125f, 0.00000f}, // 18
    {0.40625f, 0.29688f, 0.00000f}, // 19
    {0.37500f, 0.31250f, 0.00000f}, // 20
    {0.34375f, 0.32812f, 0.00000f}, // 21
    {0.31250f, 0.34375f, 0.00000f}, // 22
    {0.28125f, 0.35938f, 0.00000f}, // 23
    {0.25000f, 0.37500f, 0.00000f}, // 24
    {0.21875f, 0.39062f, 0.00000f}, // 25
    {0.18750f, 0.40625f, 0.00000f}, // 26
    {0.15625f, 0.42188f, 0.00000f}, // 27
    {0.12500f, 0.43750f, 0.00000f}, // 28
    {0.09375f, 0.45312f, 0.00000f}, // 29
    {0.06250f, 0.46875f, 0.00000f}, // 30
    {0.03125f, 0.48438f, 0.00000f}, // 31
    // Segment: Orange to Yellow
    {1.00000f, 0.50000f, 0.00000f}, // 32: Orange
    {0.96875f, 0.51562f, 0.00000f}, // 33
    {0.93750f, 0.53125f, 0.00000f}, // 34
    {0.90625f, 0.54688f, 0.00000f}, // 35
    {0.87500f, 0.56250f, 0.00000f}, // 36
    {0.84375f, 0.57812f, 0.00000f}, // 37
    {0.81250f, 0.59375f, 0.00000f}, // 38
    {0.78125f, 0.60938f, 0.00000f}, // 39
    {0.75000f, 0.62500f, 0.00000f}, // 40
    {0.71875f, 0.64062f, 0.00000f}, // 41
    {0.68750f, 0.65625f, 0.00000f}, // 42
    {0.65625f, 0.67188f, 0.00000f}, // 43
    {0.62500f, 0.68750f, 0.00000f}, // 44
    {0.59375f, 0.70312f, 0.00000f}, // 45
    {0.56250f, 0.71875f, 0.00000f}, // 46
    {0.53125f, 0.73438f, 0.00000f}, // 47
    {0.50000f, 0.75000f, 0.00000f}, // 48
    {0.46875f, 0.76562f, 0.00000f}, // 49
    {0.43750f, 0.78125f, 0.00000f}, // 50
    {0.40625f, 0.79688f, 0.00000f}, // 51
    {0.37500f, 0.81250f, 0.00000f}, // 52
    {0.34375f, 0.82812f, 0.00000f}, // 53
    {0.31250f, 0.84375f, 0.00000f}, // 54
    {0.28125f, 0.85938f, 0.00000f}, // 55
    {0.25000f, 0.87500f, 0.00000f}, // 56
    {0.21875f, 0.89062f, 0.00000f}, // 57
    {0.18750f, 0.90625f, 0.00000f}, // 58
    {0.15625f, 0.92188f, 0.00000f}, // 59
    {0.12500f, 0.93750f, 0.00000f}, // 60
    {0.09375f, 0.95312f, 0.00000f}, // 61
    {0.06250f, 0.96875f, 0.00000f}, // 62
    {0.03125f, 0.98438f, 0.00000f}, // 63
    // Segment: Yellow to Green
    {1.00000f, 1.00000f, 0.00000f}, // 64: Yellow
    {0.96875f, 1.00000f, 0.00000f}, // 65
    {0.93750f, 1.00000f, 0.00000f}, // 66
    {0.90625f, 1.00000f, 0.00000f}, // 67
    {0.87500f, 1.00000f, 0.00000f}, // 68
    {0.84375f, 1.00000f, 0.00000f}, // 69
    {0.81250f, 1.00000f, 0.00000f}, // 70
    {0.78125f, 1.00000f, 0.00000f}, // 71
    {0.75000f, 1.00000f, 0.00000f}, // 72
    {0.71875f, 1.00000f, 0.00000f}, // 73
    {0.68750f, 1.00000f, 0.00000f}, // 74
    {0.65625f, 1.00000f, 0.00000f}, // 75
    {0.62500f, 1.00000f, 0.00000f}, // 76
    {0.59375f, 1.00000f, 0.00000f}, // 77
    {0.56250f, 1.00000f, 0.00000f}, // 78
    {0.53125f, 1.00000f, 0.00000f}, // 79
    {0.50000f, 1.00000f, 0.00000f}, // 80
    {0.46875f, 1.00000f, 0.00000f}, // 81
    {0.43750f, 1.00000f, 0.00000f}, // 82
    {0.40625f, 1.00000f, 0.00000f}, // 83
    {0.37500f, 1.00000f, 0.00000f}, // 84
    {0.34375f, 1.00000f, 0.00000f}, // 85
    {0.31250f, 1.00000f, 0.00000f}, // 86
    {0.28125f, 1.00000f, 0.00000f}, // 87
    {0.25000f, 1.00000f, 0.00000f}, // 88
    {0.21875f, 1.00000f, 0.00000f}, // 89
    {0.18750f, 1.00000f, 0.00000f}, // 90
    {0.15625f, 1.00000f, 0.00000f}, // 91
    {0.12500f, 1.00000f, 0.00000f}, // 92
    {0.09375f, 1.00000f, 0.00000f}, // 93
    {0.06250f, 1.00000f, 0.00000f}, // 94
    {0.03125f, 1.00000f, 0.00000f}, // 95
    // Segment: Green to Cyan
    {0.00000f, 1.00000f, 0.00000f}, // 96: Green
    {0.00000f, 1.00000f, 0.03125f}, // 97
    {0.00000f, 1.00000f, 0.06250f}, // 98
    {0.00000f, 1.00000f, 0.09375f}, // 99
    {0.00000f, 1.00000f, 0.12500f}, // 100
    {0.00000f, 1.00000f, 0.15625f}, // 101
    {0.00000f, 1.00000f, 0.18750f}, // 102
    {0.00000f, 1.00000f, 0.21875f}, // 103
    {0.00000f, 1.00000f, 0.25000f}, // 104
    {0.00000f, 1.00000f, 0.28125f}, // 105
    {0.00000f, 1.00000f, 0.31250f}, // 106
    {0.00000f, 1.00000f, 0.34375f}, // 107
    {0.00000f, 1.00000f, 0.37500f}, // 108
    {0.00000f, 1.00000f, 0.40625f}, // 109
    {0.00000f, 1.00000f, 0.43750f}, // 110
    {0.00000f, 1.00000f, 0.46875f}, // 111
    {0.00000f, 1.00000f, 0.50000f}, // 112
    {0.00000f, 1.00000f, 0.53125f}, // 113
    {0.00000f, 1.00000f, 0.56250f}, // 114
    {0.00000f, 1.00000f, 0.59375f}, // 115
    {0.00000f, 1.00000f, 0.62500f}, // 116
    {0.00000f, 1.00000f, 0.65625f}, // 117
    {0.00000f, 1.00000f, 0.68750f}, // 118
    {0.00000f, 1.00000f, 0.71875f}, // 119
    {0.00000f, 1.00000f, 0.75000f}, // 120
    {0.00000f, 1.00000f, 0.78125f}, // 121
    {0.00000f, 1.00000f, 0.81250f}, // 122
    {0.00000f, 1.00000f, 0.84375f}, // 123
    {0.00000f, 1.00000f, 0.87500f}, // 124
    {0.00000f, 1.00000f, 0.90625f}, // 125
    {0.00000f, 1.00000f, 0.93750f}, // 126
    {0.00000f, 1.00000f, 0.96875f}, // 127
    // Segment: Cyan to Blue
    {0.00000f, 1.00000f, 1.00000f}, // 128: Cyan
    {0.00000f, 0.96875f, 1.00000f}, // 129
    {0.00000f, 0.93750f, 1.00000f}, // 130
    {0.00000f, 0.90625f, 1.00000f}, // 131
    {0.00000f, 0.87500f, 1.00000f}, // 132
    {0.00000f, 0.84375f, 1.00000f}, // 133
    {0.00000f, 0.81250f, 1.00000f}, // 134
    {0.00000f, 0.78125f, 1.00000f}, // 135
    {0.00000f, 0.75000f, 1.00000f}, // 136
    {0.00000f, 0.71875f, 1.00000f}, // 137
    {0.00000f, 0.68750f, 1.00000f}, // 138
    {0.00000f, 0.65625f, 1.00000f}, // 139
    {0.00000f, 0.62500f, 1.00000f}, // 140
    {0.00000f, 0.59375f, 1.00000f}, // 141
    {0.00000f, 0.56250f, 1.00000f}, // 142
    {0.00000f, 0.53125f, 1.00000f}, // 143
    {0.00000f, 0.50000f, 1.00000f}, // 144
    {0.00000f, 0.46875f, 1.00000f}, // 145
    {0.00000f, 0.43750f, 1.00000f}, // 146
    {0.00000f, 0.40625f, 1.00000f}, // 147
    {0.00000f, 0.37500f, 1.00000f}, // 148
    {0.00000f, 0.34375f, 1.00000f}, // 149
    {0.00000f, 0.31250f, 1.00000f}, // 150
    {0.00000f, 0.28125f, 1.00000f}, // 151
    {0.00000f, 0.25000f, 1.00000f}, // 152
    {0.00000f, 0.21875f, 1.00000f}, // 153
    {0.00000f, 0.18750f, 1.00000f}, // 154
    {0.00000f, 0.15625f, 1.00000f}, // 155
    {0.00000f, 0.12500f, 1.00000f}, // 156
    {0.00000f, 0.09375f, 1.00000f}, // 157
    {0.00000f, 0.06250f, 1.00000f}, // 158
    {0.00000f, 0.03125f, 1.00000f}, // 159
    // Segment: Blue to Violet
    {0.00000f, 0.00000f, 1.00000f}, // 160: Blue
    {0.01562f, 0.00000f, 1.00000f}, // 161
    {0.03125f, 0.00000f, 1.00000f}, // 162
    {0.04688f, 0.00000f, 1.00000f}, // 163
    {0.06250f, 0.00000f, 1.00000f}, // 164
    {0.07812f, 0.00000f, 1.00000f}, // 165
    {0.09375f, 0.00000f, 1.00000f}, // 166
    {0.10938f, 0.00000f, 1.00000f}, // 167
    {0.12500f, 0.00000f, 1.00000f}, // 168
    {0.14062f, 0.00000f, 1.00000f}, // 169
    {0.15625f, 0.00000f, 1.00000f}, // 170
    {0.17188f, 0.00000f, 1.00000f}, // 171
    {0.18750f, 0.00000f, 1.00000f}, // 172
    {0.20312f, 0.00000f, 1.00000f}, // 173
    {0.21875f, 0.00000f, 1.00000f}, // 174
    {0.23438f, 0.00000f, 1.00000f}, // 175
    {0.25000f, 0.00000f, 1.00000f}, // 176
    {0.26562f, 0.00000f, 1.00000f}, // 177
    {0.28125f, 0.00000f, 1.00000f}, // 178
    {0.29688f, 0.00000f, 1.00000f}, // 179
    {0.31250f, 0.00000f, 1.00000f}, // 180
    {0.32812f, 0.00000f, 1.00000f}, // 181
    {0.34375f, 0.00000f, 1.00000f}, // 182
    {0.35938f, 0.00000f, 1.00000f}, // 183
    {0.37500f, 0.00000f, 1.00000f}, // 184
    {0.39062f, 0.00000f, 1.00000f}, // 185
    {0.40625f, 0.00000f, 1.00000f}, // 186
    {0.42188f, 0.00000f, 1.00000f}, // 187
    {0.43750f, 0.00000f, 1.00000f}, // 188
    {0.45312f, 0.00000f, 1.00000f}, // 189
    {0.46875f, 0.00000f, 1.00000f}, // 190
    {0.48438f, 0.00000f, 1.00000f}, // 191
    // --- Grayscale Colors (Indices 192-255) ---
    {0.00000f, 0.00000f, 0.00000f}, // 192: Black
    {0.01587f, 0.01587f, 0.01587f}, // 193
    {0.03175f, 0.03175f, 0.03175f}, // 194
    {0.04762f, 0.04762f, 0.04762f}, // 195
    {0.06349f, 0.06349f, 0.06349f}, // 196
    {0.07937f, 0.07937f, 0.07937f}, // 197
    {0.09524f, 0.09524f, 0.09524f}, // 198
    {0.11111f, 0.11111f, 0.11111f}, // 199
    {0.12698f, 0.12698f, 0.12698f}, // 200
    {0.14286f, 0.14286f, 0.14286f}, // 201
    {0.15873f, 0.15873f, 0.15873f}, // 202
    {0.17460f, 0.17460f, 0.17460f}, // 203
    {0.19048f, 0.19048f, 0.19048f}, // 204
    {0.20635f, 0.20635f, 0.20635f}, // 205
    {0.22222f, 0.22222f, 0.22222f}, // 206
    {0.23810f, 0.23810f, 0.23810f}, // 207
    {0.25397f, 0.25397f, 0.25397f}, // 208
    {0.26984f, 0.26984f, 0.26984f}, // 209
    {0.28571f, 0.28571f, 0.28571f}, // 210
    {0.30159f, 0.30159f, 0.30159f}, // 211
    {0.31746f, 0.31746f, 0.31746f}, // 212
    {0.33333f, 0.33333f, 0.33333f}, // 213
    {0.34921f, 0.34921f, 0.34921f}, // 214
    {0.36508f, 0.36508f, 0.36508f}, // 215
    {0.38095f, 0.38095f, 0.38095f}, // 216
    {0.39683f, 0.39683f, 0.39683f}, // 217
    {0.41270f, 0.41270f, 0.41270f}, // 218
    {0.42857f, 0.42857f, 0.42857f}, // 219
    {0.44444f, 0.44444f, 0.44444f}, // 220
    {0.46032f, 0.46032f, 0.46032f}, // 221
    {0.47619f, 0.47619f, 0.47619f}, // 222
    {0.49206f, 0.49206f, 0.49206f}, // 223
    {0.50794f, 0.50794f, 0.50794f}, // 224
    {0.52381f, 0.52381f, 0.52381f}, // 225
    {0.53968f, 0.53968f, 0.53968f}, // 226
    {0.55556f, 0.55556f, 0.55556f}, // 227
    {0.57143f, 0.57143f, 0.57143f}, // 228
    {0.58730f, 0.58730f, 0.58730f}, // 229
    {0.60317f, 0.60317f, 0.60317f}, // 230
    {0.61905f, 0.61905f, 0.61905f}, // 231
    {0.63492f, 0.63492f, 0.63492f}, // 232
    {0.65079f, 0.65079f, 0.65079f}, // 233
    {0.66667f, 0.66667f, 0.66667f}, // 234
    {0.68254f, 0.68254f, 0.68254f}, // 235
    {0.69841f, 0.69841f, 0.69841f}, // 236
    {0.71429f, 0.71429f, 0.71429f}, // 237
    {0.73016f, 0.73016f, 0.73016f}, // 238
    {0.74603f, 0.74603f, 0.74603f}, // 239
    {0.76190f, 0.76190f, 0.76190f}, // 240
    {0.77778f, 0.77778f, 0.77778f}, // 241
    {0.79365f, 0.79365f, 0.79365f}, // 242
    {0.80952f, 0.80952f, 0.80952f}, // 243
    {0.82540f, 0.82540f, 0.82540f}, // 244
    {0.84127f, 0.84127f, 0.84127f}, // 245
    {0.85714f, 0.85714f, 0.85714f}, // 246
    {0.87302f, 0.87302f, 0.87302f}, // 247
    {0.88889f, 0.88889f, 0.88889f}, // 248
    {0.90476f, 0.90476f, 0.90476f}, // 249
    {0.92063f, 0.92063f, 0.92063f}, // 250
    {0.93651f, 0.93651f, 0.93651f}, // 251
    {0.95238f, 0.95238f, 0.95238f}, // 252
    {0.96825f, 0.96825f, 0.96825f}, // 253
    {0.98413f, 0.98413f, 0.98413f}, // 254
    {1.00000f, 1.00000f, 1.00000f}  // 255: White
};