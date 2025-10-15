#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include <memory>
#include <iostream>
#include <string>

class Player
{
public:
    Player();

    // Set camera to player
    std::shared_ptr<Camera> playerCamera;
    void setCamera(std::shared_ptr<Camera> camera) 
    {
        playerCamera = camera;
    }

    std::string name = "Russianhecker";
    glm::vec3 position;

private:
};

#endif
