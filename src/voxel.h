#ifndef VOXEL_H
#define VOXEL_H

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

class Voxel
{
    public:
        Voxel();

        static const int CHUNK_SIZE = 128;

        enum VoxelProperties : unsigned char
        {
            VoxelProperties_Null        = 0,
            VoxelProperties_Solid       = 1 << 0,
            VoxelProperties_Foliage     = 1 << 1,
            VoxelProperties_Water       = 1 << 2,
            VoxelProperties_Glass       = 1 << 3,
            VoxelProperties_Light       = 1 << 4,
            VoxelProperties_Usable      = 1 << 5,
            VoxelProperties_Unused5     = 1 << 6,
            VoxelProperties_Unused6     = 1 << 7,
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

        struct OpenGLMesh
        {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;

            int AddVertex(float x, float y, float z, float r, float g, float b);
            void AddIndex(int v0, int v1, int v2);
        };

        Chunk GenerateTestChunk();
        OpenGLMesh GenerateChunkMesh(Chunk chunk);

        void SetupRenderMesh(OpenGLMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO);
        void RenderMesh(OpenGLMesh mesh, GLuint VAO);
        void FreeRenderMesh(OpenGLMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO);
        float GetVoxelColor(VoxelData voxel_id);
        static const float voxel_colors[256][3];

    private:
};

#endif