#include "action.hpp"
#include <iostream>

void Action::validateActions(const std::vector<GameObject>& allObjects) {
    // For now, check collisions between an actor and ALL objects
    for(auto& action : actions) {
        // Compute the AABB of the actor after applying the offset
        AABB nextAABB = action.actor->computeOffsetAABB(action.offset);
        bool collided = false;
        // Check collision with all other objects
        for (const auto& obj : allObjects) {

            // Skip collision check with itself
            if (action.actor == &obj) continue;
            
            // Check if the next AABB collides with the object's AABB
            if (checkCollision(nextAABB, obj.getAABB())) {
                if(action.offset.y < 0){
                    // Downward movement was rejected, object is therefore grounded
                    if(!action.actor->isGrounded()){
                        action.actor->setGrounded(true); // Set grounded if it was not before
                        // std::cout << "Object " << action.actor->getName() << " is now grounded after downward movement.\n";
                    }
                    action.actor->setVelocity(glm::vec2(action.actor->getVelocity().x, 0.0f));
                }
                // If a collision is detected, remove the action from the action queue
                action.valid = false; // Mark the action as invalid
                collided = true;
            }
        }
        // After checking *all* objects
        if (action.offset.y > 0 && action.actor->isGrounded()) {
            action.actor->setGrounded(false);
            // std::cout << "Object " << action.actor->getName() << " is no longer grounded after upward movement.\n";
        }
        if (action.offset.y < 0 && !collided && action.actor->isGrounded()) {
            action.actor->setGrounded(false);
            // std::cout << "Object " << action.actor->getName() << " is no longer grounded after falling.\n";
        }
    }
}

void Action::processActions() {
    // Placeholder for processing logic - applies the actions to the game objects.
    // For now, this will just apply the offsets to the actors.
    for (auto& action : actions) {
        if(!action.valid) {
            continue; // Skip invalid actions
        }
        action.actor->offsetPosition(action.offset);
    }
}