#include "playerobject.hpp"
#include "debug.hpp"
#include <iostream>

void PlayerObject::setUpSensors(){
    // Set starting positions and colours for sensors
    // ASensor.position = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, -getScale().y / 2.0f - epsilon);
    // BSensor.position = getPosition() + glm::vec2(getScale().x / 2.0f + epsilon, -getScale().y / 2.0f - epsilon);
    // ASensor.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);
    // BSensor.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);
    // bottom.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);;
    // CSensor.position = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, getScale().y / 2.0f + epsilon);
    // DSensor.position = getPosition() + glm::vec2(getScale().x / 2.0f + epsilon, getScale().y / 2.0f + epsilon);
    // ESensor.position = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, 0.0f);
    // FSensor.position = getPosition() + glm::vec2(getScale().x / 2.0f + epsilon, 0.0f);

    // // Set sensor colors for debug view
    // // ASensor.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    // // BSensor.color = glm::vec4(0.0f, 1.0f, 0.8f, 1.0f);
    // bottom.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    // CSensor.color = glm::vec4(0.0f, 0.8f, 1.0f, 1.0f);
    // DSensor.color = glm::vec4(1.0f, 0.9333f, 0.0f, 1.0f);
    // ESensor.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    // FSensor.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    // // Debugging: Print sensor positions
    // std::cout << "PlayerObject sensors initialized:" << std::endl;

    leftSensor.position   = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, 0.0f);
    rightSensor.position  = getPosition() + glm::vec2( getScale().x / 2.0f + epsilon, 0.0f);
    topSensor.position    = getPosition() + glm::vec2(0.0f,  getScale().y / 2.0f + epsilon);
    bottomSensor.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);

    bottomSensor.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    topSensor.color = glm::vec4(0.0f, 0.8f, 1.0f, 1.0f);
    leftSensor.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    rightSensor.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);


}

void PlayerObject::sensorUpdate() {
    // Update sensor positions based on player position and scale
    // ASensor.position = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, -getScale().y / 2.0f - epsilon);
    // BSensor.position = getPosition() + glm::vec2(getScale().x / 2.0f + epsilon, -getScale().y / 2.0f - epsilon);
    // ASensor.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);
    // BSensor.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);
    // bottom.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);

    // CSensor.position = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, getScale().y / 2.0f + epsilon);
    // DSensor.position = getPosition() + glm::vec2(getScale().x / 2.0f + epsilon, getScale().y / 2.0f + epsilon);
    // ESensor.position = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, 0.0f);
    // FSensor.position = getPosition() + glm::vec2(getScale().x / 2.0f + epsilon, 0.0f);

    leftSensor.position   = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, 0.0f);
    rightSensor.position  = getPosition() + glm::vec2( getScale().x / 2.0f + epsilon, 0.0f);
    topSensor.position    = getPosition() + glm::vec2(0.0f,  getScale().y / 2.0f + epsilon);
    bottomSensor.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);
    
}

bool PlayerObject::tileCollision(Tilemap& tilemap, const Sensor& sensor) {
    glm::ivec2 tileIndex = tilemap.worldToTileIndex(sensor.position);
    bool isSolid = tilemap.isSolidTile(tileIndex.x, tileIndex.y);
    return isSolid; // Check if the tile at the sensor's position is solid
    // return tilemap.isSolidTile(tileIndex.x, tileIndex.y);
}

bool PlayerObject::checkIfWin(Tilemap& tilemap){
    auto playeridx = tilemap.worldToTileIndex(getPosition());
    
    // goal position is already stored as ivec2
    auto goalidx = tilemap.getGoalPos();

    if(playeridx.x == goalidx.x || playeridx.y == goalidx.y){
        std::cout<<"Player has reached the goal!\n";
        return true; // Player has reached the goal
    }
    return false;
}