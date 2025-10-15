#ifndef VOXEL_H
#define VOXEL_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "../include/FastNoiseLite/FastNoiseLite.h"

struct Chunk;
class Voxel;
class ChunkSystem;

class Voxel
{
public:
    Voxel();

    uint8_t voxelId = 1;

    struct VoxelDataNew {
        const char* name;
        bool isSolid;
        glm::vec3 color;
    };

    static const VoxelDataNew& getVoxel(uint8_t voxelId);

    struct VoxelData
    {
        unsigned char voxel_id;
        unsigned char properties;
        unsigned char light_level;
    };

    enum VoxelProperties : unsigned char
    {
        VoxelProperties_Null = 0,
        VoxelProperties_Solid = 1 << 0,
        VoxelProperties_Destructable = 1 << 1,
        VoxelProperties_Liquid = 1 << 2,
        VoxelProperties_Transparent = 1 << 3,
        VoxelProperties_Light = 1 << 4,
        VoxelProperties_Usable = 1 << 5,
        VoxelProperties_Open = 1 << 6,
        VoxelProperties_Triggered = 1 << 7,
    };

    struct VoxelColor
    {
        float r;
        float g;
        float b;
    };

    struct VoxelMesh
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        int AddVertex(float x, float y, float z, uint8_t id);
        void AddIndex(int v0, int v1, int v2);
    };

    Chunk GenerateTestChunk();
    VoxelMesh GenerateChunkMesh2(Chunk chunk);

    void SetupRenderMesh(VoxelMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO);
    void RenderMesh(VoxelMesh mesh, GLuint VAO);
    void FreeRenderMesh(VoxelMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO);
    static const float voxel_colors[256][3];

private:
    FastNoiseLite m_noise;
    static VoxelDataNew m_voxelRegistry[256];
    static bool m_initialized;
    static void init();
};

typedef long int ChunkID;

class Chunk
{
public:
    static const int CHUNK_SIZE = 128;

    Chunk(ChunkID CID, glm::ivec3 origin);
    glm::ivec3 get_origin();
    inline void SetVoxel(glm::ivec3 pos, const Voxel::VoxelData& vd);
    Voxel::VoxelData GetVoxel(glm::ivec3 pos) const;

private:
    ChunkID m_ChunkID;
    glm::ivec3 m_origin;
    std::vector<Voxel::VoxelData> m_voxel_array;
};

class ChunkSystem {
public:
    static const int WORLD_CHUNK_RADIUS = 2048;
    static const int CHUNK_HEIGHT = 4;
    
private:

};

#endif