#ifndef VOXEL_H
#define VOXEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class Voxel
{
    public:
        Voxel();
        void DrawVoxel(int voxels, Shader ourShader);
        int GetVoxelsGenerated();
        float GetVertices();

    private:
        int voxelsGenerated;
        float vertices;
        glm::vec3 cubePositions;
};
#endif