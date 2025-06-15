#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "window.hpp"
#include "shader.hpp"
#include "renderer2d.hpp"


int main(void){
    // Create a window instance
    Window window(800, 600, "OpenGL Window");

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

    // Color: opaque blue
    glm::vec4 color = glm::vec4(0.2f, 0.4f, 1.0f, 1.0f);

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

        // Projection matrix (orthographic): dynamic, based on aspect
        glm::mat4 projection = glm::ortho(
            0.0f, worldWidth,
            0.0f, worldHeight,
            -1.0f, 1.0f
        );

        // View matrix: identity for now (no camera)
        glm::mat4 view = glm::mat4(1.0f);

        // Position quad at the center of the screen
        glm::vec3 position(worldWidth / 2.0f, worldHeight / 2.0f, 0.0f);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

        renderer.beginScene(shader, view, projection); // Begin the scene
        // Draw a quad at the center of the screen
        renderer.drawQuad(shader, model, color); // Draw the quad with the specified model transform and color

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