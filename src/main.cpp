#include "helpers.hpp"

int main(void){
    Window window(1920, 1080, "OpenGL Window");
    
    Shader shader;
    Renderer2D renderer;

    if(initializeVisuals(shader, renderer) != 0){
        return -1; // Exit if initialization fails
    }

    renderer.initLine(shader); // Initialize line rendering (mostly for debug visuals)

    // --- TILEMAP SETUP ---
    std::string tilemapFile = "./assets/test.tmap";

    Tilemap tilemap = loadTilemapFromFile(tilemapFile, 1.0f); // Load tilemap with 1.0f tile size

    // Set player position in tilemap
    PlayerObject player = setupPlayerObject(tilemap, tilemap.getPlayerPosition().x, tilemap.getPlayerPosition().y);

    player.setGrounded(true); // Set the player object to be grounded initially

    // Initialize input system
    Input::initialize(window.getWindow());

    // Initialize action system
    Action actionSystem;
    Physics physicsSystem;

    float playerspeed = 1.0f;
        
    float lastFrameTime = glfwGetTime(); // seconds

    while(!window.shouldClose()){

        float currentFrameTime = glfwGetTime();
        auto frameStart = std::chrono::high_resolution_clock::now();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Poll for events
        window.pollEvents();

        if(playerInput(player, playerspeed) == -2){
            break; // Exit the loop if escape is pressed
        }

        // Check player collisions with tilemap, then move accordingly
        
        physicsSystem.playerMovementStep(player, deltaTime);
        physicsSystem.checkPlayerWorldCollisions(player, tilemap);
        if(g_debug_enabled && !player.isGrounded()){
            std::cout<<"Player has left the ground\n";
        }

        drawTilemapAndPlayer(window, renderer, shader, tilemap, player);
        
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