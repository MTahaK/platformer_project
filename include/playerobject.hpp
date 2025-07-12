#pragma once
#include <glm/glm.hpp>
#include "gameobject.hpp"
#include "tilemap.hpp"
#include "globals.hpp"

struct Sensor {
    glm::vec2 position; // Position of the sensor relative to the player
    glm::vec4 color;    // Only used for debug view 
};

// PlayerObject class inherits from GameObject
class PlayerObject : public GameObject {

    public:
        void setUpSensors();
        void sensorUpdate();

        void setGoalCount(int count) { goal_count = count; }
        void incrementGoalCount() { goal_count++; }


        bool tileCollision(Tilemap& tilemap, const Sensor& sensor);
        bool tileGoalCollision(Tilemap& tilemap, const Sensor& sensor);

        const glm::ivec2 getPlayerTileIdx(Tilemap& tilemap) const;
        int getGoalCount() const { return goal_count; }

        const Sensor& getLeftSensor() const { return leftSensor; }
        const Sensor& getRightSensor() const { return rightSensor; }
        const Sensor& getTopSensor() const { return topSensor; }
        const Sensor& getBottomSensor() const { return bottomSensor; }

        bool checkIfWin(Tilemap& tilemap);

        float goal_reach_timer = 5.0f; // Timer to track goal reach time

    private:
        // PlayerObject adds four 'sensors' to detect collisions

        Sensor leftSensor;   // Left sensor for detecting walls
        Sensor rightSensor;  // Right sensor for detecting walls
        Sensor topSensor;    // Top sensor for detecting ceilings
        Sensor bottomSensor; // Bottom sensor for detecting floors

        int goal_count = 0; // Counter for the number of sensors that detect a goal tile

};