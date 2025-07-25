#pragma once
#include <glm/glm.hpp>
#include "gameobject.hpp"
#include "playerobject.hpp"
#include "tilemap.hpp"


class Behavior {

    // Abstract base class for behaviours
    public:
        virtual ~Behavior() = default;

        // Update the behaviour for a given game object -  must be implemented
        virtual void update(GameObject& obj, float deltaTime) = 0;

        virtual void onPlayerCollision(GameObject& obj, PlayerObject& player) {}
        virtual void onTileCollision(GameObject& obj, Tilemap& tilemap) {}
        virtual void onObjectCollision(GameObject& obj, GameObject& other) {}

};

// CONCRETE BEHAVIOURS

class IdleBehavior : public Behavior {
    public:
        void update(GameObject& obj, float deltaTime) override;
};

class SpikeBehavior : public Behavior {
    // Not necessarily a 'spike', but essentially a stationary kill object
    // May extend/change to only kill when collided with on a certain surface?
    public:
        void update(GameObject& obj, float deltaTime) override;

        void onPlayerCollision(GameObject& obj, PlayerObject& player) override;
};

class DeathWallBehavior : public Behavior {
    // Behaviour for advancing death wall that starts from beginning of level geometry
    // to goal area - immediately kills player if they touch it
    public:

        DeathWallBehavior(float acceleration, glm::ivec2 startPos, glm::ivec2 endPos)
            : acceleration_(acceleration), startPos_(startPos), endPos_(endPos) {}
        void update(GameObject& obj, float deltaTime) override;

        void onPlayerCollision(GameObject& obj, PlayerObject& player) override;

    private:

        float acceleration_; // Acceleration of the death wall
        float velocity_ = 0.0f; // Current velocity of the death wall, increases based on acceleration
        glm::ivec2 startPos_; // Starting position of the death wall
        glm::ivec2 endPos_;   // Ending position of the death wall
        glm::ivec2 currentPos_; // Current position of the death wall

        // Direction is determined by the start and end positions
};