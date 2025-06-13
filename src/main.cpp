#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "window.hpp"


int main(void){
    // Create a window instance
    Window window(800, 600, "OpenGL Window");

    while(!window.shouldClose()){
        // Poll for events
        window.pollEvents();

        // Get the framebuffer size
        int width, height;
        window.getFramebufferSize(width, height);

        // Set the viewport to match the framebuffer size
        glViewport(0, 0, width, height);

        // Clear the screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap buffers
        window.swap();
    }
    // Cleanup and exit
    window.destroy();
    std::cout << "Exiting application." << std::endl;
    return 0;
}