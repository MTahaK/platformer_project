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
}



DeathWallBehavior::DeathWallBehavior(GameObject& obj, float acceleration, glm::vec2 startPos, glm::vec2 endPos, float maxSpeed)
    : acceleration_(acceleration), maxSpeed_(maxSpeed), startPos_(startPos), endPos_(endPos){
    obj.setPosition(startPos);
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
    // The wall moves in the direction of endPos from startPo s, so it MAY be either
    // entirely horizontally-moving or entirely vertically-moving
    if (startPos_.x != endPos_.x) {
        // Horizontal movement
        currentPos.x += (velocity_ * deltaTime);
        if ((startPos_.x < endPos_.x && currentPos.x >= endPos_.x) ||
            (startPos_.x > endPos_.x && currentPos.x <= endPos_.x)) {
            currentPos.x = endPos_.x; // Stop at end position
        }
    } else {
        // Vertical movement
        currentPos.y += (velocity_ * deltaTime);
        if ((startPos_.y < endPos_.y && currentPos.y >= endPos_.y) ||
            (startPos_.y > endPos_.y && currentPos.y <= endPos_.y)) {
            currentPos.y = endPos_.y; // Stop at end position
        }
    }

    obj.setPosition(glm::vec2(currentPos));
}

void DeathWallBehavior::onPlayerCollision(GameObject& obj, PlayerObject& player) {
    DEBUG_ONLY(std::cout << "Player hit death wall!" << std::endl;);
    player.setShouldDie(true);
}