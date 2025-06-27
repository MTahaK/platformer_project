#include "physics.hpp"

void playerMovementStep(GameObject& obj, float deltaTime) {
    // Horizontal pass
    float velX = obj.getAcceleration().x * deltaTime; // Calculate velocity based on acceleration
    obj.addVelocity(glm::vec2(velX, 0.0f)); // Add horizontal velocity

    if(std::abs(obj.getVelocity().x) >= MAX_VELOCITY) { // Limit horizontal velocity
        if(velX > 0.0f)
            obj.setVelocity(glm::vec2(MAX_VELOCITY, obj.getVelocity().y)); // Limit to MAX_VELOCITY
        else if(velX < 0.0f) {
            obj.setVelocity(glm::vec2(-MAX_VELOCITY, obj.getVelocity().y)); // Limit to -MAX_VELOCITY
        }
    }
    obj.applyVelocity(deltaTime); // Apply velocity to each object
    if (std::abs(obj.getVelocity().x) < 0.01f) {
        obj.setVelocity(glm::vec2(0.0f, obj.getVelocity().y));
    }
    
    // Vertical pass + gravity
    if (obj.isGrounded()) {
        obj.setVelocity(glm::vec2(obj.getVelocity().x, 0));
        return;
    }
    float velY = (obj.getAcceleration().y + gravity) * deltaTime; // Calculate vertical velocity
    
    obj.addVelocity(glm::vec2(0.0f, velY));

    if(std::abs(obj.getVelocity().y) >= MAX_VELOCITY) { // Limit vertical velocity
        if(velY > 0.0f)
            obj.setVelocity(glm::vec2(obj.getVelocity().x, MAX_VELOCITY)); // Limit to MAX_VELOCITY
        else if(velY < 0.0f) {
            obj.setVelocity(glm::vec2(obj.getVelocity().x, -MAX_VELOCITY)); // Limit to -MAX_VELOCITY
        }
    }
}