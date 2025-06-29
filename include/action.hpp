#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "gameobject.hpp"

struct QueuedAction {
    // Note: currently only supports movement actions
    glm::vec2 offset;
    GameObject* actor;           // The one initiating the action
    std::vector<GameObject*> affectedObjects; // Any objects potentially affected
    // enum class Type { Move, Attack, Push };  // For later
    // Type actionType;
    bool valid = true; // Whether the action is valid after validation checks
};

class Action{

    public:
        void addAction(const QueuedAction& action) {
            actions.push_back(action);
        }
        void validateActions(const std::vector<GameObject>& allObjects); // Process and validate actions in the queue
        void processActions(); // Execute the actions in the queue
        void clearActions() {
            actions.clear();
        }

    private:
        std::vector<QueuedAction> actions; // Queue of actions to be processed

};