#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "voxel.h"
#include "shader.h"
#include "clock.h"

std::vector<glm::vec3> cubePositions = {};

Voxel::Voxel() : last_x(0), last_z(0), chunk_count(0) {
}


void Voxel::AddVoxel(float x, float y, float z, Clock clock) {
    static float last_input_time = 0.0f;
    float current_time = clock.GetTime();

    if (current_time - last_input_time > 0.1f) {
        glm::vec3 new_voxel_position = glm::vec3(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
        if (::cubePositions.empty() || ::cubePositions.back() != new_voxel_position) {
            ::cubePositions.push_back(new_voxel_position);
            last_input_time = current_time;
        }
    }
}


Voxel::Chunk Voxel::GenerateTestChunk() {

    Chunk new_chunk(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    chunk_count += 1;

    if (chunk_count <= 1)
    {
        new_chunk.origin_x = last_x + new_chunk.dim_x / 2;
        new_chunk.origin_z = last_z + new_chunk.dim_z / 2;
    }

    for (int x = 0; x < new_chunk.dim_x; x++) {
        for (int y = 0; y < new_chunk.dim_y; y++) {
            for (int z = 0; z < new_chunk.dim_z; z++) {
                VoxelData vd;
                vd = {0, 2, 0}; // id=7, properties=2 (meshable), light=15
                new_chunk.getVoxel(x, y, z) = vd;
            }
        }
    }

    last_x = new_chunk.origin_x;
    last_z = new_chunk.origin_z;

    return new_chunk;
}

// OpenGLMesh Methods
int Voxel::OpenGLMesh::AddVertex(float x, float y, float z, float nx, float ny, float nz) {
    vertex_data.push_back(x);
    vertex_data.push_back(y);
    vertex_data.push_back(z);
    vertex_data.push_back(nx);
    vertex_data.push_back(ny);
    vertex_data.push_back(nz);
    return (vertex_data.size() / 6) - 1; // 6 floats per vertex now (3 pos, 3 normal)
}

void Voxel::OpenGLMesh::AddIndex(int v0, int v1, int v2) {
    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(v2);
}

Voxel::OpenGLMesh Voxel::GenerateChunkMesh(Chunk chunk) {
    OpenGLMesh chunk_mesh;

    for (int x_coord = 0; x_coord < CHUNK_SIZE; x_coord++) {
        for (int y_coord = 0; y_coord < CHUNK_SIZE; y_coord++) {
            for (int z_coord = 0; z_coord < CHUNK_SIZE; z_coord++) {
                if (chunk.getVoxel(x_coord, y_coord, z_coord).properties == 2) { // Only mesh 'solid' voxels
                    float fx = (float)x_coord;
                    float fy = (float)y_coord;
                    float fz = (float)z_coord;

                    // Check neighbors to determine which faces to draw (culling)
                    // Normals: +Y (top), -Y (bottom), +X (right), -X (left), +Z (back), -Z (front)

                    // Top face (+Y) Normal: (0, 1, 0)
                    if (y_coord + 1 >= CHUNK_SIZE || chunk.getVoxel(x_coord, y_coord + 1, z_coord).properties == 0) {
                        float nx=0.0f, ny=1.0f, nz=0.0f;
                        int v0 = chunk_mesh.AddVertex(fx,     fy + 1, fz,     nx,ny,nz);
                        int v1 = chunk_mesh.AddVertex(fx + 1, fy + 1, fz,     nx,ny,nz);
                        int v2 = chunk_mesh.AddVertex(fx + 1, fy + 1, fz + 1, nx,ny,nz);
                        int v3 = chunk_mesh.AddVertex(fx,     fy + 1, fz + 1, nx,ny,nz);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }

                    // Bottom face (-Y) Normal: (0, -1, 0)
                    if (y_coord - 1 < 0 || chunk.getVoxel(x_coord, y_coord - 1, z_coord).properties == 0) {
                        float nx=0.0f, ny=-1.0f, nz=0.0f;
                        int v0 = chunk_mesh.AddVertex(fx,     fy, fz,     nx,ny,nz);
                        int v1 = chunk_mesh.AddVertex(fx + 1, fy, fz,     nx,ny,nz);
                        int v2 = chunk_mesh.AddVertex(fx + 1, fy, fz + 1, nx,ny,nz);
                        int v3 = chunk_mesh.AddVertex(fx,     fy, fz + 1, nx,ny,nz);
                        chunk_mesh.AddIndex(v0, v2, v1); // Note winding for CCW from outside
                        chunk_mesh.AddIndex(v0, v3, v2);
                    }

                    // Right face (+X) Normal: (1, 0, 0)
                    if (x_coord + 1 >= CHUNK_SIZE || chunk.getVoxel(x_coord + 1, y_coord, z_coord).properties == 0) {
                        float nx=1.0f, ny=0.0f, nz=0.0f;
                        int v0 = chunk_mesh.AddVertex(fx + 1, fy,     fz,     nx,ny,nz);
                        int v1 = chunk_mesh.AddVertex(fx + 1, fy + 1, fz,     nx,ny,nz);
                        int v2 = chunk_mesh.AddVertex(fx + 1, fy + 1, fz + 1, nx,ny,nz);
                        int v3 = chunk_mesh.AddVertex(fx + 1, fy,     fz + 1, nx,ny,nz);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }

                    // Left face (-X) Normal: (-1, 0, 0)
                    if (x_coord - 1 < 0 || chunk.getVoxel(x_coord - 1, y_coord, z_coord).properties == 0) {
                        float nx=-1.0f, ny=0.0f, nz=0.0f;
                        int v0 = chunk_mesh.AddVertex(fx, fy,     fz,     nx,ny,nz);
                        int v1 = chunk_mesh.AddVertex(fx, fy + 1, fz,     nx,ny,nz);
                        int v2 = chunk_mesh.AddVertex(fx, fy + 1, fz + 1, nx,ny,nz);
                        int v3 = chunk_mesh.AddVertex(fx, fy,     fz + 1, nx,ny,nz);
                        chunk_mesh.AddIndex(v0, v2, v1); // Note winding
                        chunk_mesh.AddIndex(v0, v3, v2);
                    }

                    // Back face (+Z) Normal: (0, 0, 1)
                    if (z_coord + 1 >= CHUNK_SIZE || chunk.getVoxel(x_coord, y_coord, z_coord + 1).properties == 0) {
                        float nx=0.0f, ny=0.0f, nz=1.0f;
                        int v0 = chunk_mesh.AddVertex(fx,     fy,     fz + 1, nx,ny,nz);
                        int v1 = chunk_mesh.AddVertex(fx + 1, fy,     fz + 1, nx,ny,nz);
                        int v2 = chunk_mesh.AddVertex(fx + 1, fy + 1, fz + 1, nx,ny,nz);
                        int v3 = chunk_mesh.AddVertex(fx,     fy + 1, fz + 1, nx,ny,nz);
                        chunk_mesh.AddIndex(v0, v1, v2);
                        chunk_mesh.AddIndex(v0, v2, v3);
                    }

                    // Front face (-Z) Normal: (0, 0, -1)
                    if (z_coord - 1 < 0 || chunk.getVoxel(x_coord, y_coord, z_coord - 1).properties == 0) {
                        float nx=0.0f, ny=0.0f, nz=-1.0f;
                        int v0 = chunk_mesh.AddVertex(fx,     fy,     fz, nx,ny,nz);
                        int v1 = chunk_mesh.AddVertex(fx + 1, fy,     fz, nx,ny,nz);
                        int v2 = chunk_mesh.AddVertex(fx + 1, fy + 1, fz, nx,ny,nz);
                        int v3 = chunk_mesh.AddVertex(fx,     fy + 1, fz, nx,ny,nz);
                        chunk_mesh.AddIndex(v0, v2, v1); // Note winding
                        chunk_mesh.AddIndex(v0, v3, v2);
                    }
                }
            }
        }
    }
    return chunk_mesh;
}

void Voxel::SetupRenderMesh(OpenGLMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO) { // Pass by reference
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertex_data.size() * sizeof(float), mesh.vertex_data.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    // Position attribute (location 0)
    // Stride is 6 floats (pos_xyz + normal_xyz)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute (location 1)
    // Offset is 3 floats (after pos_xyz)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO to prevent accidental modification
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO (safe after VAO is unbound or if EBO is part of VAO state)
}

void Voxel::RenderMesh(OpenGLMesh mesh, GLuint VAO) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // Good practice to unbind after drawing
}

void Voxel::FreeRenderMesh(OpenGLMesh mesh, GLuint& VBO, GLuint& EBO, GLuint& VAO) { // Pass by reference
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    VAO = 0; // Set to 0 to indicate it's no longer a valid ID
    VBO = 0;
    EBO = 0;
}