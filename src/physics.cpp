#include "physics.hpp"

void Physics::playerMovementStep(PlayerObject& player, float deltaTime) {
    // Horizontal pass
    float velX = player.getAcceleration().x * deltaTime; // Calculate velocity based on acceleration
    player.addVelocity(glm::vec2(velX, 0.0f)); // Add horizontal velocity

    if(std::abs(player.getVelocity().x) >= MAX_VELOCITY) { // Limit horizontal velocity
        if(velX > 0.0f)
            player.setVelocity(glm::vec2(MAX_VELOCITY, player.getVelocity().y)); // Limit to MAX_VELOCITY
        else if(velX < 0.0f) {
            player.setVelocity(glm::vec2(-MAX_VELOCITY, player.getVelocity().y)); // Limit to -MAX_VELOCITY
        }
    }
    player.applyVelocity(deltaTime); // Apply velocity to each object
    if (std::abs(player.getVelocity().x) < 0.01f) {
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
    }
    
    // Vertical pass + gravity
    if (player.isGrounded()) {
        player.setVelocity(glm::vec2(player.getVelocity().x, 0));
        player.sensorUpdate();
        return;
    }
    float velY = (player.getAcceleration().y + gravity) * deltaTime; // Calculate vertical velocity
    
    player.addVelocity(glm::vec2(0.0f, velY));

    if(std::abs(player.getVelocity().y) >= MAX_VELOCITY) { // Limit vertical velocity
        if(velY > 0.0f)
            player.setVelocity(glm::vec2(player.getVelocity().x, MAX_VELOCITY)); // Limit to MAX_VELOCITY
        else if(velY < 0.0f) {
            player.setVelocity(glm::vec2(player.getVelocity().x, -MAX_VELOCITY)); // Limit to -MAX_VELOCITY
        }
    }

    // Update sensor positions
    player.sensorUpdate();
}

void Physics::checkPlayerWorldCollisions(PlayerObject& player, Tilemap& tilemap) {
    float epsilon = 0.001f; // Small offset to avoid sticking to tiles
    // Check collisions with the tilemap using sensors
    if (player.tileCollision(tilemap, player.getLeftSensor())) {
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y)); // Stop horizontal movement
        player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y)); // Reset horizontal acceleration
        player.offsetPosition(glm::vec2(epsilon, 0.0f)); // Offset to avoid sticking
    }
    if (player.tileCollision(tilemap, player.getRightSensor())) {
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y));
        player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y));
        player.offsetPosition(glm::vec2(-epsilon, 0.0f));
    }
    if (player.tileCollision(tilemap, player.getTopSensor())) {
        player.setVelocity(glm::vec2(player.getVelocity().x, 0.0f));
        player.offsetPosition(glm::vec2(0.0f, -epsilon));
    }
    if (player.tileCollision(tilemap, player.getBottomSensor())) {
        player.setGrounded(true);
        player.setVelocity(glm::vec2(player.getVelocity().x, 0.0f));
        player.offsetPosition(glm::vec2(0.0f, epsilon));
    }
}