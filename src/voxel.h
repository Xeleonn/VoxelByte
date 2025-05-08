#ifndef VOXEL_H
#define VOXEL_H

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h> // For GLuint
#include "shader.h" // Assuming Shader class is used (it was in main.cpp)
#include "clock.h"  // For AddVoxel method, keep as is

#define CHUNK_SIZE 128 // Define chunk size

class Voxel {
public:
    Voxel();
    int last_x, last_z;
    int chunk_count;

    // Structure for voxel data within a chunk
    struct VoxelData {
        unsigned char id;
        unsigned char properties; // e.g., 0 for air, 1 for solid (generic), 2 for meshable solid
        unsigned char light_level;
    };

    // Structure for a chunk
    struct Chunk {
        int origin_x = 5, origin_y = 2, origin_z = 14;
        std::vector<VoxelData> voxel_grid_data;
        int dim_x, dim_y, dim_z;

        Chunk(int x = CHUNK_SIZE, int y = CHUNK_SIZE, int z = CHUNK_SIZE) : dim_x(x), dim_y(y), dim_z(z) {
            voxel_grid_data.resize(static_cast<size_t>(dim_x) * dim_y * dim_z);
        }

        // Helper to access elements like a 3D array (const version)
        const VoxelData& getVoxel(int x, int y, int z) const {
            return voxel_grid_data[static_cast<size_t>(z) * dim_y * dim_x + static_cast<size_t>(y) * dim_x + x];
        }

        // Helper to access elements like a 3D array (non-const version)
        VoxelData& getVoxel(int x, int y, int z) {
            return voxel_grid_data[static_cast<size_t>(z) * dim_y * dim_x + static_cast<size_t>(y) * dim_x + x];
        }
    };

    // Structure for OpenGL mesh data
    struct OpenGLMesh {
        std::vector<float> vertex_data; // Interleaved: x,y,z,nx,ny,nz
        std::vector<unsigned int> indices;

        // Adds a vertex with position and normal, returns its index
        int AddVertex(float x, float y, float z, float nx, float ny, float nz);
        // Adds three indices to form a triangle
        void AddIndex(int v0, int v1, int v2);
    };

    void AddVoxel(float x, float y, float z, Clock clock);

    Chunk GenerateTestChunk();
    OpenGLMesh GenerateChunkMesh(Chunk chunk);

    // OpenGL resource management for meshes
    // Pass VBO, EBO, VAO by reference to modify the originals in main
    void SetupRenderMesh(OpenGLMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO);
    void RenderMesh(OpenGLMesh mesh, GLuint VAO); // VAO is just an ID here
    void FreeRenderMesh(OpenGLMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO);

private:
};

#endif // VOXEL_H