#ifndef VOXEL_H
#define VOXEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "shader.h"
#include "clock.h"

class Voxel
{
    public:
        Voxel();
        void AddVoxel(float x, float y, float z, Clock clock);
        void DrawVoxel(int voxels, Shader shader);
        int GetVoxelsGenerated();

<<<<<<< Updated upstream
=======
        enum class VoxelProperties : int
        {
            VoxelFlagNull            = 0,
            VoxelFlagLOD             = 1 << 0,
            VoxelFlagSolid           = 1 << 1,
            VoxelFlagInteractable    = 1 << 2
        };

        struct VoxelData
        {
            uint8_t color_idx;
            int properties;
            int unused;
        };

        struct Chunk
        {
            int origin_x;
            int origin_y;
            int origin_z;
            VoxelData voxel_grid[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
        };

        struct OpenGLMesh
        {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;
            
            int AddVertex(float a, float b, float c);
            void AddIndex(int v0, int v1, int v2);
        };

        Chunk GenerateTestChunk();
        OpenGLMesh GenerateChunkMesh(Chunk chunk);
        
        void SetupRenderMesh(OpenGLMesh mesh, GLuint VBO, GLuint EBO, GLuint VAO);
        void RenderMesh(OpenGLMesh mesh, GLuint VAO);
        void FreeRenderMesh(OpenGLMesh mesh, GLuint VBO, GLuint EBO, GLuint VAO);

>>>>>>> Stashed changes
    private:
        int voxelsGenerated;
        float vertices;
        std::vector<glm::vec3> cubePositions;
};
#endif