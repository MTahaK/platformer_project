#include "helpers.hpp"

constexpr double targetFPS = 120.0;
constexpr double targetFrameTime = 1.0 / targetFPS; // ~0.016666... seconds

const float gravity = -8.0f;

int main(void){
    Window window(1920, 1080, "OpenGL Window");
    
    Shader shader;
    Renderer2D renderer;

    if(initializeVisuals(shader, renderer) != 0){
        return -1; // Exit if initialization fails
    }

    // --- INITIAL WORLD DIMENSIONS (used only to set initial object positions) ---
    float initialWorldHeight = 5.0f;
    float initialAspectRatio = 1920.0f / 1080.0f;
    float initialWorldWidth = initialWorldHeight * initialAspectRatio;

    // Set up game objects
    std::vector<GameObject> objects = setupObjects(initialWorldHeight, initialWorldWidth);

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
        // Input::update();

        // glm::vec2 playerposoffset(0.0f, 0.0f); // Player position offset, used for movement 

        if(playerInput(objects[1], playerspeed) == -2){
            break; // Exit the loop if escape is pressed
        }
        std::ostringstream oss;
        glm::vec2 velocity = objects[1].getVelocity();
        oss << "Player speed (X, Y): (" << velocity.x << ", " << velocity.y << ")";
        std::string output = oss.str();
        output.resize(50, ' '); // Ensure line is at least 50 characters long
        std::cout << "\r" << output << std::flush;
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

        for (auto& obj : objects) {
            if(obj.getName() == "Ground") {
                continue; // Skip ground object for velocity application
            }
            if (!obj.isGrounded()) {
                obj.addVelocity(glm::vec2(0.0f, gravity * deltaTime));
            }

            // Convert velocity into a queued action
            glm::vec2 displacement = obj.getVelocity() * deltaTime;
            // if (displacement != glm::vec2(0.0f)) {
            //     actionSystem.addAction({
            //         .offset = displacement,
            //         .actor = &obj,
            //         .affectedObjects = {}
            //     });
            // }
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