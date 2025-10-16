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

    struct VoxelData {
        const char* name;
        glm::vec3 color;
        bool solid;
        bool destructible;
        bool interactable;
        bool flammable;
        bool affectedByGravity;
    };

    static const VoxelData& GetVoxelData(uint8_t voxelId);

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
    static const float voxelColors[256][3];

private:
    FastNoiseLite m_noise;
    static VoxelData m_voxelRegistry[256];
    static bool m_initialized;
    static void init();
};

typedef long int ChunkID;

class Chunk
{
public:
    static const int CHUNK_SIZE = 128;

    Chunk(ChunkID CID, glm::ivec3 origin);
    glm::ivec3 getOrigin();
    inline void SetVoxel(glm::ivec3 pos, const uint8_t& vd);
    uint8_t GetVoxel(glm::ivec3 pos) const;

private:
    ChunkID m_chunkID;
    glm::ivec3 m_origin;
    std::vector<uint8_t> m_voxelArray;
};

class ChunkSystem {
public:
    static const int WORLD_CHUNK_RADIUS = 2048;
    static const int CHUNK_HEIGHT = 4;
    
private:

};

#endif