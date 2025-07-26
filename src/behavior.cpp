#include <iostream>
#include "behavior.hpp"
#include "gameobject.hpp"
#include "playerobject.hpp"
#include "tilemap.hpp"
#include "debug.hpp"
#include "globals.hpp"


void IdleBehavior::update(GameObject& obj, float deltaTime) {
    // Nothing
}

void KillBehavior::update(GameObject& obj, float deltaTime) {
    // Nothing
}

void KillBehavior::onPlayerCollision(GameObject& obj, PlayerObject& player) {
    DEBUG_ONLY(std::cout << "Player hit kill object!" << std::endl;);
    // Handle player death
    player.setShouldDie(true);
}

DeathWallBehavior::DeathWallBehavior(GameObject& obj, float acceleration, glm::vec2 startPos, glm::vec2 endPos)
    : acceleration_(acceleration), startPos_(startPos), endPos_(endPos){
    obj.setPosition(startPos);
    // Suppose startPos = (1, 2) and endPos = (4, 2), then direction_ = (3, 0).
    // If startPos = (1, 2) and endPos = (1, 5), then direction_ = (0, 3).
    // Normalize to get the final direction vector
    direction_ = glm::vec2(endPos - startPos);
    if (direction_ != glm::vec2(0)) {
        direction_ = glm::normalize(direction_);
    }
}



DeathWallBehavior::DeathWallBehavior(GameObject& obj, float acceleration, glm::vec2 startPos, glm::vec2 endPos, float maxSpeed)
    : acceleration_(acceleration), maxSpeed_(maxSpeed), startPos_(startPos), endPos_(endPos){
    obj.setPosition(startPos);
    direction_ = endPos - startPos;
    if (direction_ != glm::vec2(0)) {
        direction_ = glm::normalize(direction_);
    }
}

void DeathWallBehavior::update(GameObject& obj, float deltaTime) {
    // Compute velocity from acceleration
    velocity_ += acceleration_ * deltaTime;
    
    // Apply speed limit if one is set
    if (maxSpeed_ > 0.0f && velocity_ > maxSpeed_) {
        velocity_ = maxSpeed_;
    }

    auto currentPos = obj.getPosition();
    // Simple death wall only moves along one axis; progresses from startPos to endPos
    // The wall moves in the direction of endPos from startPos, so it may be either
    // entirely horizontally-moving or entirely vertically-moving
    
    currentPos += (velocity_ * deltaTime) * direction_;

    obj.setPosition(currentPos);
}

void DeathWallBehavior::onPlayerCollision(GameObject& obj, PlayerObject& player) {
    DEBUG_ONLY(std::cout << "Player hit death wall!" << std::endl;);
    player.setShouldDie(true);
}