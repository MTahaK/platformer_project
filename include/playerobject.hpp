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

        bool tileCollision(Tilemap& tilemap, const Sensor& sensor);
        // Getters for sensors
        // const Sensor& getASensor() const { return ASensor; }
        // const Sensor& getBSensor() const { return BSensor; }
        // const Sensor& getBottomSensor() const { return bottom; }
        // const Sensor& getCSensor() const { return CSensor; }
        // const Sensor& getDSensor() const { return DSensor; }
        // const Sensor& getESensor() const { return ESensor; }
        // const Sensor& getFSensor() const { return FSensor; }

        const Sensor& getLeftSensor() const { return leftSensor; }
        const Sensor& getRightSensor() const { return rightSensor; }
        const Sensor& getTopSensor() const { return topSensor; }
        const Sensor& getBottomSensor() const { return bottomSensor; }

        bool checkIfWin(Tilemap& tilemap);

    private:
        // PlayerObject adds six 'sensors' to detect collisions
        // Their positioning and naming take after the sensors used in
        // the classic Sonic games:
        // A-B: Ground collisions
        // C-D: Ceiling collisions
        // E-F: Wall collisions

        // Sensor ASensor; // Left sensor for ground collision
        // Sensor BSensor; // Right sensor for ground collision
        // Sensor bottom;
        // Sensor CSensor; // Left sensor for ceiling collision
        // Sensor DSensor; // Right sensor for ceiling collision
        // Sensor ESensor; // Left sensor for wall collision
        // Sensor FSensor; // Right sensor for wall collision

        Sensor leftSensor;   // Left sensor for detecting walls
        Sensor rightSensor;  // Right sensor for detecting walls
        Sensor topSensor;    // Top sensor for detecting ceilings
        Sensor bottomSensor; // Bottom sensor for detecting floors

};