#pragma once
#include <glm/glm.hpp>
#include "gameobject.hpp"

struct Sensor {
    glm::vec2 position; // Position of the sensor relative to the player
};

// PlayerObject class inherits from GameObject
class PlayerObject : public GameObject {

    private:
        // PlayerObject adds four 'sensors' to detect collisions
        Sensor leftSensor;   // Left sensor for detecting walls
        Sensor rightSensor;  // Right sensor for detecting walls
        Sensor topSensor;    // Top sensor for detecting ceilings
        Sensor bottomSensor; // Bottom sensor for detecting floors

};