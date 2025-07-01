#pragma once
#include <glm/glm.hpp>
#include "gameobject.hpp"
#include "tilemap.hpp"

struct Sensor {
    glm::vec2 position; // Position of the sensor relative to the player
};

const float epsilon = 0.0001f; // Small offset to avoid sticking to tiles

// PlayerObject class inherits from GameObject
class PlayerObject : public GameObject {

    public:

        void sensorUpdate() {
            // Update sensor positions based on player position and scale
            leftSensor.position   = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, 0.0f);
            rightSensor.position  = getPosition() + glm::vec2( getScale().x / 2.0f + epsilon, 0.0f);
            topSensor.position    = getPosition() + glm::vec2(0.0f,  getScale().y / 2.0f + epsilon);
            bottomSensor.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);
        }

        bool tileCollision(Tilemap& tilemap, const Sensor& sensor);
        // Getters for sensors
        const Sensor& getLeftSensor() const { return leftSensor; }
        const Sensor& getRightSensor() const { return rightSensor; }
        const Sensor& getTopSensor() const { return topSensor; }
        const Sensor& getBottomSensor() const { return bottomSensor; }
    private:
        // PlayerObject adds four 'sensors' to detect collisions
        Sensor leftSensor;   // Left sensor for detecting walls
        Sensor rightSensor;  // Right sensor for detecting walls
        Sensor topSensor;    // Top sensor for detecting ceilings
        Sensor bottomSensor; // Bottom sensor for detecting floors

};