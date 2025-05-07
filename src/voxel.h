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

    private:
        int voxelsGenerated;
        float vertices;
        std::vector<glm::vec3> cubePositions;
};
#endif