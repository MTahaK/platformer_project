#pragma once
#include <vector>
#include "gameobject.hpp"

struct QueuedAction {
    // Note: currently only supports movement actions
    glm::vec2 offset;
    GameObject* actor;           // The one initiating the action
    std::vector<GameObject*> affectedObjects; // Any objects potentially affected
    // enum class Type { Move, Attack, Push };  // For later
    // Type actionType;
};

class Action{

    public:
        void init();
        void addAction(const QueuedAction& action) {
            actions.push_back(action);
        }
        void validateActions(); // Process and validate actions in the queue
        void processActions(); // Execute the actions in the queue
        void clearActions() {
            actions.clear();
        }

    private:
        std::vector<QueuedAction> actions; // Queue of actions to be processed

}