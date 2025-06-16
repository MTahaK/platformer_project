#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "window.hpp"
#include "shader.hpp"
#include "renderer2d.hpp"
#include "gameobject.hpp"

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


    while(!window.shouldClose()){
        // Poll for events
        window.pollEvents();
        // Get current framebuffer size
        int fbWidth, fbHeight;
        window.getFramebufferSize(fbWidth, fbHeight);
        float aspect = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);

        // Define a fixed vertical size for the in-game "world"
        float worldHeight = 5.0f;
        float worldWidth = worldHeight * aspect;

        std::vector<GameObject> objects = {
            { {worldWidth * 0.3f, worldHeight * 0.5f}, {0.2f, 0.4f, 1.0f, 1.0f} }, // blue, left
            { {worldWidth * 0.5f, worldHeight * 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f} }, // green, center
            { {worldWidth * 0.7f, worldHeight * 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f} }  // red, right
        };

        // Projection matrix (orthographic): dynamic, based on aspect
        glm::mat4 projection = glm::ortho(
            0.0f, worldWidth,
            0.0f, worldHeight,
            -1.0f, 1.0f
        );

        // View matrix: identity for now (no camera)
        glm::mat4 view = glm::mat4(1.0f);

        renderer.beginScene(shader, view, projection); // Begin the scene
        // Draw a quad at the center of the screen
        for(const auto& object : objects){
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(object.getPosition(), 0.0f));
            renderer.drawQuad(shader, model, object.getColor());
        }

        // Swap buffers
        window.swap();
    }
    // Cleanup and exit
    renderer.shutdown();
    shader.shutdown();
    window.destroy();
    std::cout << "Exiting application." << std::endl;
    return 0;
}