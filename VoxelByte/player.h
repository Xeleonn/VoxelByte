#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <memory>
#include <iostream>
#include <string>

class Player
{
public:
    Player();

    // Set camera to player


    std::string name = "Player";
    glm::vec3 position;

private:
};

#endif
