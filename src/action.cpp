#include "action.hpp"


void Action::validateActions() {
    // Placeholder for validation logic - primarily, checks for collisions
    // or other game rules that might prevent an action from being valid.
    // Since we have no collision logic yet, this is a no-op.
}

void Action::processActions() {
    // Placeholder for processing logic - applies the actions to the game objects.
    // For now, this will just apply the offsets to the actors.
    for (const auto& action : actions) {
        action.actor->offsetPosition(action.offset);
        // Future: Apply effects to affectedObjects if needed
    }
}