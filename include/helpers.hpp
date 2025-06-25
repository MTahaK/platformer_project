#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>
#include "window.hpp"
#include "shader.hpp"
#include "renderer2d.hpp"
#include "gameobject.hpp"
#include "input.hpp"
#include "action.hpp"

int initializeVisuals(Shader& shader, Renderer2D& renderer){
    // Loader shaders
    if(!shader.load("shaders/vertex.glsl", "shaders/fragment.glsl")){
        std::cerr << "Failed to load shaders. Exiting application." << std::endl;
        return -1; // Exit if shader loading fails
    }

    // Initialize renderer
    if(!renderer.init(shader)){
        std::cerr << "Failed to initialize renderer. Exiting application." << std::endl;
        return -1; // Exit if renderer initialization fails
    }

    return 0;
}

std::vector<GameObject> setupObjects(float& worldHeight, float& worldWidth) {

    // --- Create game objects ONCE (they persist across frames) ---
    std::vector<GameObject> objects = {
        { {worldWidth * 0.3f, worldHeight * 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} }, // blue, left
        { {worldWidth * 0.5f, worldHeight * 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f} }, // green, center
        { {worldWidth * 0.7f, worldHeight * 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f} }  // red, right
    };
    
    objects[0].setName("Left Object");
    objects[1].setName("Player Object");
    objects[2].setName("Right Object");

       // Add a ground object (static platform)
    GameObject ground(
        { worldWidth / 2.0f, 0.45f },         // centered horizontally, near bottom
        { worldWidth*3, 1.0f },               // full width, 1 unit tall
        0.0f,
        { 0.5f, 0.25f, 0.0f, 1.0f }                // brown-ish color
    );
    ground.setName("Ground");
    objects.push_back(ground);

    return objects;
}

int playerInput(GameObject& player, float& playerspeed) {
    // Key polling for basic movement
    Input::update();

    if(Input::isKeyPressed(GLFW_KEY_LEFT) || Input::isKeyPressed(GLFW_KEY_A)){
        player.setVelocity(glm::vec2(-playerspeed, player.getVelocity().y)); // Set velocity directly
    }
    else if(Input::isKeyPressed(GLFW_KEY_RIGHT) || Input::isKeyPressed(GLFW_KEY_D)){
        player.setVelocity(glm::vec2(playerspeed, player.getVelocity().y)); // Set velocity directly
    }
    else{
        // If no horizontal movement keys are pressed, reset player velocity to zero
        player.setVelocity(glm::vec2(0.0f, player.getVelocity().y)); // Keep vertical velocity
    }
    if(Input::isKeyJustPressed(GLFW_KEY_UP) || Input::isKeyPressed(GLFW_KEY_W)){
        if(player.isGrounded()){
            // player.setGrounded(false); // Set player to not grounded
            player.addVelocity(glm::vec2(0.0f, 5.0f)); // Apply upward velocity
        }
    }
    if(Input::isKeyPressed(GLFW_KEY_DOWN) || Input::isKeyPressed(GLFW_KEY_S)){
        // Increase freefall speed
        if(!player.isGrounded()){
            player.setVelocity(glm::vec2(player.getVelocity().x, -10.0f)); // Apply downward velocity
        }
    }
    if(Input::isKeyJustPressed(GLFW_KEY_LEFT_SHIFT)){
        playerspeed = 2.0f;
    }
    if(Input::isKeyJustReleased(GLFW_KEY_LEFT_SHIFT)){
        playerspeed = 1.0f;
    }
    if(Input::isKeyJustPressed(GLFW_KEY_ESCAPE)){
        return -2; // "break" signal to exit the loop
    }
    return 0;
}
