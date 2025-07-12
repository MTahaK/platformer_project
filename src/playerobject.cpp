#include "playerobject.hpp"
#include "debug.hpp"
#include <iostream>

void PlayerObject::setUpSensors(){
    leftSensor.position   = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, 0.0f);
    rightSensor.position  = getPosition() + glm::vec2( getScale().x / 2.0f + epsilon, 0.0f);
    topSensor.position    = getPosition() + glm::vec2(0.0f,  getScale().y / 2.0f + epsilon);
    bottomSensor.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);

    // Colours for debug
    bottomSensor.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    topSensor.color = glm::vec4(0.0f, 0.8f, 1.0f, 1.0f);
    leftSensor.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    rightSensor.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);


}

void PlayerObject::sensorUpdate() {
    leftSensor.position   = getPosition() + glm::vec2(-getScale().x / 2.0f - epsilon, 0.0f);
    rightSensor.position  = getPosition() + glm::vec2( getScale().x / 2.0f + epsilon, 0.0f);
    topSensor.position    = getPosition() + glm::vec2(0.0f,  getScale().y / 2.0f + epsilon);
    bottomSensor.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - epsilon);
    
}

const glm::ivec2 PlayerObject::getPlayerTileIdx(Tilemap& tilemap) const {
    return tilemap.worldToTileIndex(getPosition());
}

bool PlayerObject::tileCollision(Tilemap& tilemap, const Sensor& sensor) {
    glm::ivec2 tileIndex = tilemap.worldToTileIndex(sensor.position);
    bool isSolid = tilemap.isSolidTile(tileIndex.x, tileIndex.y);
    return isSolid; // Check if the tile at the sensor's position is solid
    // return tilemap.isSolidTile(tileIndex.x, tileIndex.y);
}

bool PlayerObject::tileGoalCollision(Tilemap& tilemap, const Sensor& sensor) {
    glm::ivec2 tileIndex = tilemap.worldToTileIndex(sensor.position);
    bool isGoal = tilemap.isGoalTile(tileIndex.x, tileIndex.y);
    if(isGoal){
        if(goal_count < 4) goal_count++;
    } else if(goal_count > 0) {
        goal_count--; // Decrease goal count if the player is not on a goal tile
    } else if (goal_count < 0) {
        goal_count = 0; // Ensure goal count does not go negative
    } else{
        goal_count = 0; // Reset goal count if the player is not on a goal tile
    }
    return isGoal; // Check if the tile at the sensor's position is a goal tile
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