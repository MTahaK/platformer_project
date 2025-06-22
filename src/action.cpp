#include "action.hpp"
#include <iostream>


void Action::validateActions(const std::vector<GameObject>& allObjects) {
    // For now, check collisions between an actor and ALL objects
    for(const auto& action : actions) {

        // Check collision with all other objects
        for (const auto& obj : allObjects) {

            // Skip collision check with itself
            if (action.actor == &obj) continue;

            if (checkCollision(action.actor->getAABB(), obj.getAABB())) {
                // Collision detected - handle it here
                // Set colour to red for the actor
                action.actor->setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color
                std::cout << "Collision detected between actor: " << action.actor->getName() << " and object: " << obj.getName() << std::endl;
            }
        }
    }
}

void Action::processActions() {
    // Placeholder for processing logic - applies the actions to the game objects.
    // For now, this will just apply the offsets to the actors.
    for (const auto& action : actions) {
        action.actor->offsetPosition(action.offset);
        // Recompute AABB after moving
        action.actor->computeAABB();
    }
}