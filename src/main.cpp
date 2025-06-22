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

constexpr double targetFPS = 120.0;
constexpr double targetFrameTime = 1.0 / targetFPS; // ~0.016666... seconds


int main(void){
    // Create a window instance
    Window window(1920, 1080, "OpenGL Window");
    
    // Create shader instance, load shaders, and check for success
    Shader shader;
    if(!shader.load("shaders/vertex.glsl", "shaders/fragment.glsl")){
        std::cerr << "Failed to load shaders. Exiting application." << std::endl;
        return -1; // Exit if shader loading fails
    }

    // Set up renderer
    Renderer2D renderer;
    if(!renderer.init(shader)){
        std::cerr << "Failed to initialize renderer. Exiting application." << std::endl;
        return -1; // Exit if renderer initialization fails
    }

    
    // --- INITIAL WORLD DIMENSIONS (used only to set initial object positions) ---
    float initialWorldHeight = 5.0f;
    float initialAspectRatio = 1920.0f / 1080.0f;
    float initialWorldWidth = initialWorldHeight * initialAspectRatio;

    // --- Create game objects ONCE (they persist across frames) ---
    std::vector<GameObject> objects = {
        { {initialWorldWidth * 0.3f, initialWorldHeight * 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} }, // blue, left
        { {initialWorldWidth * 0.5f, initialWorldHeight * 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f} }, // green, center
        { {initialWorldWidth * 0.7f, initialWorldHeight * 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f} }  // red, right
    };
    
    objects[0].setName("Left Object");
    objects[1].setName("Player Object");
    objects[2].setName("Right Object");
    // Initialize input system
    Input::initialize(window.getWindow());

    // Initialize action system
    Action actionSystem;

    int leftdir = -1;
    int rightdir = 1;
    float playerspeed = 1.0f;
        
    float lastFrameTime = glfwGetTime(); // seconds

    while(!window.shouldClose()){
        float currentFrameTime = glfwGetTime();
        auto frameStart = std::chrono::high_resolution_clock::now();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        // Poll for events
        window.pollEvents();

        // Key polling for basic movement
        Input::update();

        glm::vec2 playerposoffset(0.0f, 0.0f); // Player position offset, used for movement 

        if(Input::isKeyPressed(GLFW_KEY_LEFT) || Input::isKeyPressed(GLFW_KEY_A)){
            // playerposoffset.x -= 1.0f * deltaTime; // Move left
            // Create QueuedAction for left movement and add to action system
            actionSystem.addAction({
                .offset = glm::vec2(-playerspeed * deltaTime, 0.0f), // Small step left
                .actor = &objects[1], // The player object
                .affectedObjects = {}
            });
        }
        if(Input::isKeyPressed(GLFW_KEY_RIGHT) || Input::isKeyPressed(GLFW_KEY_D)){
            // playerposoffset.x += 1.0f * deltaTime; // Move right
            actionSystem.addAction({
                .offset = glm::vec2(playerspeed * deltaTime, 0.0f), // Small step left
                .actor = &objects[1], // The player object
                .affectedObjects = {}
            });
        }
        if(Input::isKeyPressed(GLFW_KEY_UP) || Input::isKeyPressed(GLFW_KEY_W)){
            // playerposoffset.y += 1.0f * deltaTime; // Move up
            actionSystem.addAction({
                .offset = glm::vec2(0.0f, playerspeed * deltaTime), // Small step left
                .actor = &objects[1], // The player object
                .affectedObjects = {}
            });
        }
        if(Input::isKeyPressed(GLFW_KEY_DOWN) || Input::isKeyPressed(GLFW_KEY_S)){
            // playerposoffset.y -= 1.0f * deltaTime; // Move down
            actionSystem.addAction({
                .offset = glm::vec2(0.0f, -playerspeed * deltaTime), // Small step left
                .actor = &objects[1], // The player object
                .affectedObjects = {}
            });
        }
        if(Input::isKeyJustPressed(GLFW_KEY_LEFT_SHIFT)){
            playerspeed = 2.0f;
        }
        if(Input::isKeyJustReleased(GLFW_KEY_LEFT_SHIFT)){
            playerspeed = 1.0f;
        }
        if(Input::isKeyJustPressed(GLFW_KEY_ESCAPE)){
            break; // Exit the loop
        }

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
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(object.getPosition(), 0.0f));
            renderer.drawQuad(shader, model, object.getColor());
        }

        // Swap buffers
        window.swap();

        // Process actions in the action system
        actionSystem.validateActions(objects); // Validate actions (e.g., check for collisions)
        actionSystem.processActions(); // Apply the actions to the game objects
        actionSystem.clearActions(); // Clear the action queue for next frame
        
        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = frameEnd - frameStart;
    }
    // Cleanup and exit
    renderer.shutdown();
    shader.shutdown();
    window.destroy();
    std::cout << "Exiting application." << std::endl;
    return 0;
}