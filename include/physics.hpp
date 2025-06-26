#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include "gameobject.hpp"

const float MAX_VELOCITY = 5.0f; // Maximum velocity limit
class Physics {

    public:
        void movementStep(std::vector<GameObject>& objects, float deltaTime) {
            for (auto& obj : objects) {
                if(obj.getName() == "Player Object"){
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
                }
            }
        }
};