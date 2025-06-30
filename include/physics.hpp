#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include "gameobject.hpp"
#include "playerobject.hpp"

const float gravity = -8.0f;
const float MAX_VELOCITY = 5.0f; // Maximum velocity limit
class Physics {

    public:

        void playerMovementStep(PlayerObject& player, float deltaTime);
        void checkPlayerWorldCollisions(GameObject& obj);    // Will use tilemap-sensor method
        void checkPlayerEntityCollisions(GameObject& obj, const std::vector<GameObject>& entities); // May end up unused

};