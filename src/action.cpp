#include "action.hpp"
#include <iostream>

void Action::validateActions(const std::vector<GameObject>& allObjects) {
    // For now, check collisions between an actor and ALL objects
    for(auto& action : actions) {
        // Compute the AABB of the actor after applying the offset
        AABB nextAABB = action.actor->computeOffsetAABB(action.offset);
        // Check collision with all other objects
        for (const auto& obj : allObjects) {

            // Skip collision check with itself
            if (action.actor == &obj) continue;
            
            // Check if the next AABB collides with the object's AABB
            if (checkCollision(nextAABB, obj.getAABB())) {
                // If a collision is detected, remove the action from the action queue
                action.valid = false; // Mark the action as invalid
            }
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