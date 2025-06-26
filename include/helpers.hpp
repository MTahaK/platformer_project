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
#include "physics.hpp"

constexpr double targetFPS = 120.0;
constexpr double targetFrameTime = 1.0 / targetFPS; // ~0.016666... seconds

const float gravity = -8.0f;
const float movementAccel = 5.0f; // Acceleration applied when moving left/right
const float slowdownAccel = 8.0f; // Acceleration applied when slowing down

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
        // player.setVelocity(glm::vec2(-playerspeed, player.getVelocity().y)); // Set velocity directly
        player.setAcceleration(glm::vec2(-movementAccel, player.getAcceleration().y)); // Set acceleration directly
    }
    else if(Input::isKeyPressed(GLFW_KEY_RIGHT) || Input::isKeyPressed(GLFW_KEY_D)){
        // player.setVelocity(glm::vec2(playerspeed, player.getVelocity().y)); // Set velocity directly
        player.setAcceleration(glm::vec2(movementAccel, player.getAcceleration().y)); // Set acceleration directly
    }
    else{
        // If no horizontal movement keys are pressed, reset player velocity to zero
        // player.setVelocity(glm::vec2(0.0f, player.getVelocity().y)); // Keep vertical velocity
        // player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y)); // Reset horizontal acceleration
        // Simulate mid-air drag
        if(player.getVelocity().x > 0.0f){
            player.setAcceleration(glm::vec2(-slowdownAccel, player.getAcceleration().y)); // Apply leftward acceleration
        }
        else if(player.getVelocity().x < 0.0f){
            player.setAcceleration(glm::vec2(slowdownAccel, player.getAcceleration().y)); // Apply rightward acceleration
        }
        else{
            player.setAcceleration(glm::vec2(0.0f, player.getAcceleration().y)); // No horizontal acceleration
        }
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

    std::ostringstream oss;
    glm::vec2 velocity = player.getVelocity();
    glm::vec2 acceleration = player.getAcceleration(); // Assuming this method exists

    oss << "Vel (X,Y): (" << velocity.x << ", " << velocity.y << ")  "
        << "Acc (X,Y): (" << acceleration.x << ", " << acceleration.y << ")";
    std::string output = oss.str();
    output.resize(80, ' '); // Pad to overwrite old output cleanly

    std::cout << "\r" << output << std::flush;
    return 0;
}

void miscMovement(std::vector<GameObject>& objects, float initialWorldWidth, int& leftdir, int& rightdir, Action& actionSystem) {
    // Secondary object movement

    // If the left object hits the left world boundary, reverse direction
    if(objects[0].getPosition().x <= 0.0f){
        leftdir = 1; // Reverse direction
    }
    // If it hits the right world boundary, reverse direction
    if(objects[0].getPosition().x >= initialWorldWidth){
        leftdir = -1; // Reverse direction
    }
    // If the right object hits the right world boundary, reverse direction
    if(objects[2].getPosition().x >= initialWorldWidth){
        rightdir = -1; // Reverse direction
    }
    // If it hits the left world boundary, reverse direction
    if(objects[2].getPosition().x <= 0.0f){
        rightdir = 1; // Reverse direction
    }
    
    actionSystem.addAction({
            .offset = glm::vec2(leftdir*0.001f, 0.0f), // Move left
            .actor = &objects[0], // The left object
            .affectedObjects = {}
        });
    actionSystem.addAction({
        .offset = glm::vec2(rightdir*0.001f, 0.0f), // Move right
        .actor = &objects[2], // The right object
        .affectedObjects = {}
    });

    // objects[0].setVelocity(glm::vec2(leftdir * 0.001f, objects[0].getVelocity().y)); // Move left
    // objects[2].setVelocity(glm::vec2(rightdir * 0.001f, objects[2].getVelocity().y)); // Move right
}

void queueActions(std::vector<GameObject>& objects, Action& actionSystem, float deltaTime) {
    for (auto& obj : objects) {
        if(obj.getName() == "Ground" || obj.getName() == "Player Object") {
            // Decouple player movement from action queue
            continue; // Skip ground object for velocity application
        }
        if (!obj.isGrounded()) {
            obj.addVelocity(glm::vec2(0.0f, gravity * deltaTime));
        }

        // Convert velocity into a queued action
        glm::vec2 displacement = obj.getVelocity() * deltaTime;

        // Queue x and y displacements separately
        if (displacement.x != 0.0f) {
            actionSystem.addAction({
                .offset = glm::vec2(displacement.x, 0.0f),
                .actor = &obj,
                .affectedObjects = {}
            });
        }
        if (displacement.y != 0.0f) {
            actionSystem.addAction({
                .offset = glm::vec2(0.0f, displacement.y),
                .actor = &obj,
                .affectedObjects = {}
            });
        }
    }
}
void drawStep(Window& window, Renderer2D& renderer, Shader& shader, const std::vector<GameObject>& objects) {
    
    // Get current framebuffer size
    int fbWidth, fbHeight;
    window.getFramebufferSize(fbWidth, fbHeight);
    float aspect = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);

    // Define a fixed vertical size for the in-game "world"
    float worldHeight = 5.0f;
    float worldWidth = worldHeight * aspect;

    // Projection matrix (orthographic): dynamic, based on aspect
    glm::mat4 projection = glm::ortho(
        0.0f, worldWidth,
        0.0f, worldHeight,
        -1.0f, 1.0f
    );

    // View matrix: identity for now (no camera)
    glm::mat4 view = glm::mat4(1.0f);

    renderer.beginScene(shader, view, projection); // Begin the scene

    for(const auto& object : objects){
        glm::mat4 model = object.getModelMatrix();
        renderer.drawQuad(shader, model, object.getColor());
    }

    // Swap buffers
    window.swap();
}

void applyActions( std::vector<GameObject>& objects, Action& actionSystem) {
    // Process actions in the action system
    actionSystem.validateActions(objects); // Validate actions (e.g., check for collisions)
    actionSystem.processActions(); // Apply the actions to the game objects
    actionSystem.clearActions(); // Clear the action queue for next frame
}