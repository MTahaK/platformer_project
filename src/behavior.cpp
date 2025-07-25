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

DeathWallBehavior::DeathWallBehavior(float acceleration, glm::ivec2 startPos, glm::ivec2 endPos)
    : acceleration_(acceleration), startPos_(startPos), endPos_(endPos), currentPos_(startPos) {
    // Initialize currentPos_ to starting position
    // 
    // Acceleration tuning guide:
    // - Small values (0.1 - 0.5): Very gradual acceleration, gives players lots of time
    // - Medium values (0.5 - 2.0): Noticeable pressure without being overwhelming  
    // - Large values (2.0+): Aggressive acceleration, creates high pressure gameplay
    //
    // Example: acceleration = 1.0f means wall gains 1 unit/second of speed every second
    // After 5 seconds: velocity = 5 units/second
    // After 10 seconds: velocity = 10 units/second
}

DeathWallBehavior::DeathWallBehavior(float acceleration, glm::ivec2 startPos, glm::ivec2 endPos, float maxSpeed)
    : acceleration_(acceleration), maxSpeed_(maxSpeed), startPos_(startPos), endPos_(endPos), currentPos_(startPos) {};

void DeathWallBehavior::update(GameObject& obj, float deltaTime) {
    // Compute velocity from acceleration
    velocity_ += acceleration_ * deltaTime;
    
    // Apply speed limit if one is set
    if (maxSpeed_ > 0.0f && velocity_ > maxSpeed_) {
        velocity_ = maxSpeed_;
    }

    // Simple death wall only moves along one axis; progresses from startPos to endPos
    // The wall moves in the direction of endPos from startPos, so it MAY be either
    // entirely horizontally-moving or entirely vertically-moving
    if (startPos_.x != endPos_.x) {
        // Horizontal movement
        currentPos_.x += static_cast<int>(velocity_ * deltaTime);
        if ((startPos_.x < endPos_.x && currentPos_.x >= endPos_.x) ||
            (startPos_.x > endPos_.x && currentPos_.x <= endPos_.x)) {
            currentPos_.x = endPos_.x; // Stop at end position
        }
    } else {
        // Vertical movement
        currentPos_.y += static_cast<int>(velocity_ * deltaTime);
        if ((startPos_.y < endPos_.y && currentPos_.y >= endPos_.y) ||
            (startPos_.y > endPos_.y && currentPos_.y <= endPos_.y)) {
            currentPos_.y = endPos_.y; // Stop at end position
        }
    }

    obj.setPosition(glm::vec2(currentPos_));
}

void DeathWallBehavior::onPlayerCollision(GameObject& obj, PlayerObject& player) {
    DEBUG_ONLY(std::cout << "Player hit death wall!" << std::endl;);
    player.setShouldDie(true);
}