#pragma once
#include <glm/glm.hpp>
#include <memory>

class GameObject;
class PlayerObject;
class Tilemap;


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

class KillBehavior : public Behavior {

    public:
        void update(GameObject& obj, float deltaTime) override;

        void onPlayerCollision(GameObject& obj, PlayerObject& player) override;
};

class DeathWallBehavior : public Behavior {
    // Behaviour for advancing death wall that starts from beginning of level geometry
    // to goal area - immediately kills player if they touch it
    public:

        DeathWallBehavior(GameObject& obj, float acceleration, glm::vec2 startPos, glm::vec2 endPos);
        // Optional: constructor with max speed limit
        DeathWallBehavior(GameObject& obj, float acceleration, glm::vec2 startPos, glm::vec2 endPos, float maxSpeed);

        void update(GameObject& obj, float deltaTime) override;

        void onPlayerCollision(GameObject& obj, PlayerObject& player) override;

    private:

        float acceleration_; // Acceleration of the death wall
        float velocity_ = 0.0f; // Current velocity of the death wall, increases based on acceleration
        float maxSpeed_ = -1.0f; // Maximum speed (-1 means no limit)
        glm::vec2 startPos_; // Starting position of the death wall
        glm::vec2 endPos_;   // Ending position of the death wall

        // Direction is determined by the start and end positions
        glm::vec2 direction_;
};

class MovingPlatformBehavior : public Behavior {
    // When the player is on the top surface of the platform, the platform's velocity
    // is added to the player's velocity
    public:
        MovingPlatformBehavior(GameObject& obj, float speed, glm::vec2 direction);

        void update(GameObject& obj, float deltaTime) override;

        void onPlayerCollision(GameObject& obj, PlayerObject& player) override;
    private:
        float speed_;
        glm::vec2 direction_;
};