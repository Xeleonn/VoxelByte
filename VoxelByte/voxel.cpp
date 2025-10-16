#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <unordered_map>
#include "voxel.h"
#include "shader.h"
#include "clock.h"

Voxel::Voxel()
{
    m_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
}

bool Voxel::m_initialized = false;
Voxel::VoxelData Voxel::m_voxelRegistry[256];

void Voxel::init() {
    if (m_initialized) return;
    m_initialized = true;

    // Fill with defaults
    for (int i = 0; i < 256; i++) {
        m_voxelRegistry[i] = 
        { 
            .name = "undefined", 
            .color = {1.0f, 0.0f, 1.0f}, 
            .solid = true, 
            .destructible = true, 
            .interactable = false, 
            .flammable = false, 
            .affectedByGravity = false 
        };
    }

    // Voxel ID registry
    m_voxelRegistry[0] = 
    { 
        .name = "air", 
        .color = {1.0f, 1.0f, 1.0f}, 
        .solid = false,
        .destructible = false,
        .interactable = false,
        .flammable = false,
        .affectedByGravity = false
    };
    m_voxelRegistry[1] = 
    { 
        .name = "grass", 
        .color = {0.0f, 0.43f, 0.0f}, 
        .solid = true,
        .destructible = true,
        .interactable = false,
        .flammable = false,
        .affectedByGravity = false
    };
    m_voxelRegistry[2] = 
    { 
        .name = "dirt", 
        .color = {0.30f, 0.15f, 0.0f}, 
        .solid = true,
        .destructible = true,
        .interactable = false,
        .flammable = false,
        .affectedByGravity = false
    };
    m_voxelRegistry[3] = 
    { 
        .name = "stone", 
        .color = {0.36f, 0.36f, 0.36f}, 
        .solid = true,
        .destructible = true,
        .interactable = false,
        .flammable = false,
        .affectedByGravity = false
    };
}

const Voxel::VoxelData& Voxel::GetVoxelData(uint8_t voxelId) {
    init();
    return m_voxelRegistry[voxelId];
}

// ----------<[ CHUNK CLASS IMPLEMENTATION ]>----------
Chunk::Chunk(ChunkID CID, glm::ivec3 origin)
{
    m_chunkID = CID;
    m_origin = origin;

    m_voxelArray.resize(Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE);
}

glm::ivec3 Chunk::getOrigin()
{
    return m_origin;
}

void Chunk::SetVoxel(glm::ivec3 pos, const uint8_t& id)
{
    m_voxelArray.at(pos.x * CHUNK_SIZE * CHUNK_SIZE + pos.y * CHUNK_SIZE + pos.z) = id;
}

uint8_t Chunk::GetVoxel(glm::ivec3 pos) const
{
    return m_voxelArray.at(pos.x * CHUNK_SIZE * CHUNK_SIZE + pos.y * CHUNK_SIZE + pos.z);
}

Chunk Voxel::GenerateTestChunk()
{
    std::vector<float> noiseData(Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE);

    Chunk newChunk(0, glm::ivec3(0, 0, 0));

    size_t noise_idx = 0;
    for (int x = 0; x < Chunk::CHUNK_SIZE; x++)
    {
        for (int y = 0; y < Chunk::CHUNK_SIZE; y++)
        {
            noiseData[noise_idx++] = m_noise.GetNoise((float)x, (float)y);
        }
    }

    for (int x = 0; x < Chunk::CHUNK_SIZE; x++)
    {
        for (int y = 0; y < Chunk::CHUNK_SIZE; y++)
        {
            for (int z = 0; z < Chunk::CHUNK_SIZE; z++)
            {
                uint8_t v_id;

                if ((noiseData[x * Chunk::CHUNK_SIZE + z] + 1.0f) * 64.0f > y)
                    v_id = 1;
                else
                    v_id = 0;

                newChunk.SetVoxel(glm::vec3(x, y, z), v_id);
            }
        }
    }

    return newChunk;
}

int Voxel::VoxelMesh::AddVertex(float x, float y, float z, uint8_t id)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(GetVoxelData(id).color.x);
    vertices.push_back(GetVoxelData(id).color.y);
    vertices.push_back(GetVoxelData(id).color.z);
    return static_cast<int>((vertices.size() / 6) - 1);
}

void Voxel::VoxelMesh::AddIndex(int v0, int v1, int v2) {
    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(v2);
}

Voxel::VoxelMesh Voxel::GenerateChunkMesh2(Chunk chunk)
{
    VoxelMesh chunkMesh;
    // Sweep over each axis (X, Y and Z)
    for (int d = 0; d < 3; d++)
    {
        int i = 0, j = 0, k = 0, l = 0, w = 0, h = 0;
        int u = (d + 1) % 3;
        int v = (d + 2) % 3;
        int* x = new int[3] {0};
        int* q = new int[3] {0};

        bool* mask = new bool[Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE] {false};
        q[d] = 1;

        // Check each slice of the chunk one at a time
        for (x[d] = -1; x[d] < Chunk::CHUNK_SIZE;)
        {
            // Compute the mask
            int n = 0;
            for (x[v] = 0; x[v] < Chunk::CHUNK_SIZE; x[v]++)
            {
                for (x[u] = 0; x[u] < Chunk::CHUNK_SIZE; x[u]++)
                {
                    // q determines the direction (X, Y or Z) that we are searching
                    // m.IsBlockAt(x,y,z) takes global map positions and returns true if a block exists there


                    bool blockCurrent;
                    bool blockCompare;

                    if (0 <= x[d]) {
                        uint8_t voxelId = chunk.GetVoxel(glm::ivec3(x[0], x[1], x[2]));
                        blockCurrent = !Voxel::GetVoxelData(voxelId).solid;
                    }
                    else {
                        blockCurrent = true;
                    }

                    if (x[d] < (Chunk::CHUNK_SIZE - 1)) {
                        uint8_t voxelId = chunk.GetVoxel(glm::ivec3(x[0] + q[0], x[1] + q[1], x[2] + q[2]));
                        blockCompare = !Voxel::GetVoxelData(voxelId).solid;
                    }
                    else {
                        blockCompare = true;
                    }

                    // The mask is set to true if there is a visible face between two blocks,
                    //   i.e. both aren't empty and both aren't blocks
                    mask[n] = (blockCurrent != blockCompare);
                    n += 1;
                }
            }

            x[d]++;

            n = 0;

            // Generate a mesh from the mask using lexicographic ordering,      
            //   by looping over each block in this slice of the chunk
            for (j = 0; j < Chunk::CHUNK_SIZE; j++)
            {
                for (i = 0; i < Chunk::CHUNK_SIZE;)
                {
                    if (mask[n])
                    {
                        // Compute the width of this quad and store it in w                        
                        //   This is done by searching along the current axis until mask[n + w] is false
                        for (w = 1; ((i + w) < Chunk::CHUNK_SIZE) && mask[n + w]; w++) {}

                        // Compute the height of this quad and store it in h                        
                        //   This is done by checking if every block next to this row (range 0 to w) is also part of the mask.
                        //   For example, if w is 5 we currently have a quad of dimensions 1 x 5. To reduce triangle count,
                        //   greedy meshing will attempt to expand this quad out to CHUNK_SIZE x 5, but will stop if it reaches a hole in the mask

                        bool done = false;
                        for (h = 1; (j + h) < Chunk::CHUNK_SIZE; h++)
                        {
                            // Check each block next to this quad
                            for (k = 0; k < w; k++)
                            {
                                // If there's a hole in the mask, exit
                                if (!mask[n + k + h * Chunk::CHUNK_SIZE])
                                {
                                    done = true;
                                    break;
                                }
                            }

                            if (done)
                                break;
                        }

                        x[u] = i;
                        x[v] = j;

                        // du and dv determine the size and orientation of this face
                        int* du = new int[3] {0};
                        du[u] = w;

                        int* dv = new int[3] {0};
                        dv[v] = h;

                        // Create a quad for this face. Colour, normal or textures are not stored in this block vertex format.

                        int v0 = chunkMesh.AddVertex((float)(x[0]), (float)(x[1]), (float)(x[2]), voxelId);
                        int v1 = chunkMesh.AddVertex((float)(x[0] + du[0]), (float)(x[1] + du[1]), (float)(x[2] + du[2]), voxelId);
                        int v2 = chunkMesh.AddVertex((float)(x[0] + dv[0]), (float)(x[1] + dv[1]), (float)(x[2] + dv[2]), voxelId);
                        int v3 = chunkMesh.AddVertex((float)(x[0] + du[0] + dv[0]), (float)(x[1] + du[1] + dv[1]), (float)(x[2] + du[2] + dv[2]), voxelId);
                        chunkMesh.AddIndex(v0, v1, v2);
                        chunkMesh.AddIndex(v1, v2, v3);

                        // Clear this part of the mask, so we don't add duplicate faces
                        for (l = 0; l < h; l++)
                            for (k = 0; k < w; k++)
                                mask[n + k + l * Chunk::CHUNK_SIZE] = false;

                        // Increment counters and continue
                        i += w;
                        n += w;
                    }
                    else
                    {
                        i++;
                        n++;
                    }
                }
            }
        }
    }

    return chunkMesh;
}

void Voxel::SetupRenderMesh(VoxelMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO)
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

void Voxel::RenderMesh(VoxelMesh mesh, GLuint VAO)
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Voxel::FreeRenderMesh(VoxelMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO)
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    VAO = 0;
    VBO = 0;
    EBO = 0;
}

const float Voxel::voxelColors[256][3] = {
    // --- Red (Bright to Dark) ---
    {1.00000f, 0.00000f, 0.00000f}, // 0: Bright Red
    {0.97419f, 0.00000f, 0.00000f}, // 1
    {0.94839f, 0.00000f, 0.00000f}, // 2
    {0.92258f, 0.00000f, 0.00000f}, // 3
    {0.89677f, 0.00000f, 0.00000f}, // 4
    {0.87097f, 0.00000f, 0.00000f}, // 5
    {0.84516f, 0.00000f, 0.00000f}, // 6
    {0.81935f, 0.00000f, 0.00000f}, // 7
    {0.79355f, 0.00000f, 0.00000f}, // 8
    {0.76774f, 0.00000f, 0.00000f}, // 9
    {0.74194f, 0.00000f, 0.00000f}, // 10
    {0.71613f, 0.00000f, 0.00000f}, // 11
    {0.69032f, 0.00000f, 0.00000f}, // 12
    {0.66452f, 0.00000f, 0.00000f}, // 13
    {0.63871f, 0.00000f, 0.00000f}, // 14
    {0.61290f, 0.00000f, 0.00000f}, // 15
    {0.58710f, 0.00000f, 0.00000f}, // 16
    {0.56129f, 0.00000f, 0.00000f}, // 17
    {0.53548f, 0.00000f, 0.00000f}, // 18
    {0.50968f, 0.00000f, 0.00000f}, // 19
    {0.48387f, 0.00000f, 0.00000f}, // 20
    {0.45806f, 0.00000f, 0.00000f}, // 21
    {0.43226f, 0.00000f, 0.00000f}, // 22
    {0.40645f, 0.00000f, 0.00000f}, // 23
    {0.38065f, 0.00000f, 0.00000f}, // 24
    {0.35484f, 0.00000f, 0.00000f}, // 25
    {0.32903f, 0.00000f, 0.00000f}, // 26
    {0.30323f, 0.00000f, 0.00000f}, // 27
    {0.27742f, 0.00000f, 0.00000f}, // 28
    {0.25161f, 0.00000f, 0.00000f}, // 29
    {0.22581f, 0.00000f, 0.00000f}, // 30
    {0.20000f, 0.00000f, 0.00000f}, // 31: Dark Red
    // --- Orange (Bright to Dark) ---
    {1.00000f, 0.60000f, 0.00000f}, // 32: Bright Orange
    {0.97742f, 0.58548f, 0.00000f}, // 33
    {0.95484f, 0.57097f, 0.00000f}, // 34
    {0.93226f, 0.55645f, 0.00000f}, // 35
    {0.90968f, 0.54194f, 0.00000f}, // 36
    {0.88710f, 0.52742f, 0.00000f}, // 37
    {0.86452f, 0.51290f, 0.00000f}, // 38
    {0.84194f, 0.49839f, 0.00000f}, // 39
    {0.81935f, 0.48387f, 0.00000f}, // 40
    {0.79677f, 0.46935f, 0.00000f}, // 41
    {0.77419f, 0.45484f, 0.00000f}, // 42
    {0.75161f, 0.44032f, 0.00000f}, // 43
    {0.72903f, 0.42581f, 0.00000f}, // 44
    {0.70645f, 0.41129f, 0.00000f}, // 45
    {0.68387f, 0.39677f, 0.00000f}, // 46
    {0.66129f, 0.38226f, 0.00000f}, // 47
    {0.63871f, 0.36774f, 0.00000f}, // 48
    {0.61613f, 0.35323f, 0.00000f}, // 49
    {0.59355f, 0.33871f, 0.00000f}, // 50
    {0.57097f, 0.32419f, 0.00000f}, // 51
    {0.54839f, 0.30968f, 0.00000f}, // 52
    {0.52581f, 0.29516f, 0.00000f}, // 53
    {0.50323f, 0.28065f, 0.00000f}, // 54
    {0.48065f, 0.26613f, 0.00000f}, // 55
    {0.45806f, 0.25161f, 0.00000f}, // 56
    {0.43548f, 0.23710f, 0.00000f}, // 57
    {0.41290f, 0.22258f, 0.00000f}, // 58
    {0.39032f, 0.20806f, 0.00000f}, // 59
    {0.36774f, 0.19355f, 0.00000f}, // 60
    {0.34516f, 0.17903f, 0.00000f}, // 61
    {0.32258f, 0.16452f, 0.00000f}, // 62
    {0.30000f, 0.15000f, 0.00000f}, // 63: Dark Orange
    // --- Yellow (Bright to Dark) ---
    {1.00000f, 1.00000f, 0.00000f}, // 64: Bright Yellow
    {0.97742f, 0.97742f, 0.00000f}, // 65
    {0.95484f, 0.95484f, 0.00000f}, // 66
    {0.93226f, 0.93226f, 0.00000f}, // 67
    {0.90968f, 0.90968f, 0.00000f}, // 68
    {0.88710f, 0.88710f, 0.00000f}, // 69
    {0.86452f, 0.86452f, 0.00000f}, // 70
    {0.84194f, 0.84194f, 0.00000f}, // 71
    {0.81935f, 0.81935f, 0.00000f}, // 72
    {0.79677f, 0.79677f, 0.00000f}, // 73
    {0.77419f, 0.77419f, 0.00000f}, // 74
    {0.75161f, 0.75161f, 0.00000f}, // 75
    {0.72903f, 0.72903f, 0.00000f}, // 76
    {0.70645f, 0.70645f, 0.00000f}, // 77
    {0.68387f, 0.68387f, 0.00000f}, // 78
    {0.66129f, 0.66129f, 0.00000f}, // 79
    {0.63871f, 0.63871f, 0.00000f}, // 80
    {0.61613f, 0.61613f, 0.00000f}, // 81
    {0.59355f, 0.59355f, 0.00000f}, // 82
    {0.57097f, 0.57097f, 0.00000f}, // 83
    {0.54839f, 0.54839f, 0.00000f}, // 84
    {0.52581f, 0.52581f, 0.00000f}, // 85
    {0.50323f, 0.50323f, 0.00000f}, // 86
    {0.48065f, 0.48065f, 0.00000f}, // 87
    {0.45806f, 0.45806f, 0.00000f}, // 88
    {0.43548f, 0.43548f, 0.00000f}, // 89
    {0.41290f, 0.41290f, 0.00000f}, // 90
    {0.39032f, 0.39032f, 0.00000f}, // 91
    {0.36774f, 0.36774f, 0.00000f}, // 92
    {0.34516f, 0.34516f, 0.00000f}, // 93
    {0.32258f, 0.32258f, 0.00000f}, // 94
    {0.30000f, 0.30000f, 0.00000f}, // 95: Dark Yellow
    // --- Green (Bright to Dark) ---
    {0.00000f, 1.00000f, 0.00000f}, // 96: Bright Green
    {0.00000f, 0.97419f, 0.00000f}, // 97
    {0.00000f, 0.94839f, 0.00000f}, // 98
    {0.00000f, 0.92258f, 0.00000f}, // 99
    {0.00000f, 0.89677f, 0.00000f}, // 100
    {0.00000f, 0.87097f, 0.00000f}, // 101
    {0.00000f, 0.84516f, 0.00000f}, // 102
    {0.00000f, 0.81935f, 0.00000f}, // 103
    {0.00000f, 0.79355f, 0.00000f}, // 104
    {0.00000f, 0.76774f, 0.00000f}, // 105
    {0.00000f, 0.74194f, 0.00000f}, // 106
    {0.00000f, 0.71613f, 0.00000f}, // 107
    {0.00000f, 0.69032f, 0.00000f}, // 108
    {0.00000f, 0.66452f, 0.00000f}, // 109
    {0.00000f, 0.63871f, 0.00000f}, // 110
    {0.00000f, 0.61290f, 0.00000f}, // 111
    {0.00000f, 0.58710f, 0.00000f}, // 112
    {0.00000f, 0.56129f, 0.00000f}, // 113
    {0.00000f, 0.53548f, 0.00000f}, // 114
    {0.00000f, 0.50968f, 0.00000f}, // 115
    {0.00000f, 0.48387f, 0.00000f}, // 116
    {0.00000f, 0.45806f, 0.00000f}, // 117
    {0.00000f, 0.43226f, 0.00000f}, // 118
    {0.00000f, 0.40645f, 0.00000f}, // 119
    {0.00000f, 0.38065f, 0.00000f}, // 120
    {0.00000f, 0.35484f, 0.00000f}, // 121
    {0.00000f, 0.32903f, 0.00000f}, // 122
    {0.00000f, 0.30323f, 0.00000f}, // 123
    {0.00000f, 0.27742f, 0.00000f}, // 124
    {0.00000f, 0.25161f, 0.00000f}, // 125
    {0.00000f, 0.22581f, 0.00000f}, // 126
    {0.00000f, 0.20000f, 0.00000f}, // 127: Dark Green
    // --- Blue (Bright to Dark) ---
    {0.00000f, 0.00000f, 1.00000f}, // 128: Bright Blue
    {0.00000f, 0.00000f, 0.97419f}, // 129
    {0.00000f, 0.00000f, 0.94839f}, // 130
    {0.00000f, 0.00000f, 0.92258f}, // 131
    {0.00000f, 0.00000f, 0.89677f}, // 132
    {0.00000f, 0.00000f, 0.87097f}, // 133
    {0.00000f, 0.00000f, 0.84516f}, // 134
    {0.00000f, 0.00000f, 0.81935f}, // 135
    {0.00000f, 0.00000f, 0.79355f}, // 136
    {0.00000f, 0.00000f, 0.76774f}, // 137
    {0.00000f, 0.00000f, 0.74194f}, // 138
    {0.00000f, 0.00000f, 0.71613f}, // 139
    {0.00000f, 0.00000f, 0.69032f}, // 140
    {0.00000f, 0.00000f, 0.66452f}, // 141
    {0.00000f, 0.00000f, 0.63871f}, // 142
    {0.00000f, 0.00000f, 0.61290f}, // 143
    {0.00000f, 0.00000f, 0.58710f}, // 144
    {0.00000f, 0.00000f, 0.56129f}, // 145
    {0.00000f, 0.00000f, 0.53548f}, // 146
    {0.00000f, 0.00000f, 0.50968f}, // 147
    {0.00000f, 0.00000f, 0.48387f}, // 148
    {0.00000f, 0.00000f, 0.45806f}, // 149
    {0.00000f, 0.00000f, 0.43226f}, // 150
    {0.00000f, 0.00000f, 0.40645f}, // 151
    {0.00000f, 0.00000f, 0.38065f}, // 152
    {0.00000f, 0.00000f, 0.35484f}, // 153
    {0.00000f, 0.00000f, 0.32903f}, // 154
    {0.00000f, 0.00000f, 0.30323f}, // 155
    {0.00000f, 0.00000f, 0.27742f}, // 156
    {0.00000f, 0.00000f, 0.25161f}, // 157
    {0.00000f, 0.00000f, 0.22581f}, // 158
    {0.00000f, 0.00000f, 0.20000f}, // 159: Dark Blue
    // --- Violet (Bright to Dark) ---
    {0.60000f, 0.10000f, 0.70000f}, // 160: Bright Violet
    {0.58710f, 0.09677f, 0.68548f}, // 161
    {0.57419f, 0.09355f, 0.67097f}, // 162
    {0.56129f, 0.09032f, 0.65645f}, // 163
    {0.54839f, 0.08710f, 0.64194f}, // 164
    {0.53548f, 0.08387f, 0.62742f}, // 165
    {0.52258f, 0.08065f, 0.61290f}, // 166
    {0.50968f, 0.07742f, 0.59839f}, // 167
    {0.49677f, 0.07419f, 0.58387f}, // 168
    {0.48387f, 0.07097f, 0.56935f}, // 169
    {0.47097f, 0.06774f, 0.55484f}, // 170
    {0.45806f, 0.06452f, 0.54032f}, // 171
    {0.44516f, 0.06129f, 0.52581f}, // 172
    {0.43226f, 0.05806f, 0.51129f}, // 173
    {0.41935f, 0.05484f, 0.49677f}, // 174
    {0.40645f, 0.05161f, 0.48226f}, // 175
    {0.39355f, 0.04839f, 0.46774f}, // 176
    {0.38065f, 0.04516f, 0.45323f}, // 177
    {0.36774f, 0.04194f, 0.43871f}, // 178
    {0.35484f, 0.03871f, 0.42419f}, // 179
    {0.34194f, 0.03548f, 0.40968f}, // 180
    {0.32903f, 0.03226f, 0.39516f}, // 181
    {0.31613f, 0.02903f, 0.38065f}, // 182
    {0.30323f, 0.02581f, 0.36613f}, // 183
    {0.29032f, 0.02258f, 0.35161f}, // 184
    {0.27742f, 0.01935f, 0.33710f}, // 185
    {0.26452f, 0.01613f, 0.32258f}, // 186
    {0.25161f, 0.01290f, 0.30806f}, // 187
    {0.23871f, 0.00968f, 0.29355f}, // 188
    {0.22581f, 0.00645f, 0.27903f}, // 189
    {0.21290f, 0.00323f, 0.26452f}, // 190
    {0.20000f, 0.00000f, 0.25000f}, // 191: Dark Violet
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