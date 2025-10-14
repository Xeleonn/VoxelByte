#ifndef VOXEL_H
#define VOXEL_H

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include "../include/FastNoiseLite/FastNoiseLite.h"

class Voxel
{
public:
    Voxel();

    static const int CHUNK_SIZE = 128;

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

    struct VoxelData
    {
        unsigned char voxel_id;
        unsigned char properties;
        unsigned char light_level;
    };

    struct VoxelColor
    {
        float r;
        float g;
        float b;
    };

    struct Chunk
    {
        int origin_x = 0, origin_y = 0, origin_z = 0;
        std::vector<VoxelData> voxel_array;
        float color[3];

        Chunk();
        VoxelData& GetVoxel(int pos_x, int pos_y, int pos_z);
        VoxelColor GetColor(VoxelData voxel);
    };

    struct VoxelMesh
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        int AddVertex(float x, float y, float z, float r, float g, float b);
        void AddIndex(int v0, int v1, int v2);
    };

    Chunk GenerateTestChunk();
    VoxelMesh GenerateChunkMesh(Chunk chunk);
    VoxelMesh GenerateChunkMesh2(Chunk chunk);

    void SetupRenderMesh(VoxelMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO);
    void RenderMesh(VoxelMesh mesh, GLuint VAO);
    void FreeRenderMesh(VoxelMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO);
    //float GetVoxelColor(VoxelData voxel_id);
    int ColorHeight(int y, int chunk_size);
    static const float voxel_colors[256][3];

private:
    FastNoiseLite m_noise;
};

#endif