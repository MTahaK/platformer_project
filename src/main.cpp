#include "helpers.hpp"

int main(void){
    Window window(1920, 1080, "OpenGL Window");
    
    Shader shader;
    Renderer2D renderer;

    if(initializeVisuals(shader, renderer) != 0){
        return -1; // Exit if initialization fails
    }

    // --- INITIAL WORLD DIMENSIONS (used only to set initial object positions) ---
    // float initialWorldHeight = 5.0f;
    // float initialAspectRatio = 1920.0f / 1080.0f;
    // float initialWorldWidth = initialWorldHeight * initialAspectRatio;

    // --- TILEMAP SETUP ---
    std::string tilemapFile = "./assets/test.tilemap";

    Tilemap tilemap = loadTilemapFromFile(tilemapFile, 1.0f); // Load tilemap with 1.0f tile size

    // Verify that tilemap loaded correctly by printing it out to console, line by line
    std::cout<<"Loaded tilema data:"<<std::endl;
    for(int y = tilemap.getHeight() - 1; y >= 0; --y) {
        for(int x = 0; x < tilemap.getWidth(); ++x) {
            Tile& tile = tilemap.getTile(x, y);
            std::cout << (tile.tileType.type == TileEnum::SOLID ? '#' : '.');
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    // Set player position in tilemap
    PlayerObject player = setupPlayerObject(tilemap, tilemap.getPlayerPosition().x, tilemap.getPlayerPosition().y);

    // Set up game objects
    // std::vector<GameObject> objects = setupObjects(initialWorldHeight, initialWorldWidth);

    // objects[1].setGrounded(true); // Set the player object to be grounded initially
    player.setGrounded(true); // Set the player object to be grounded initially

    // Initialize input system
    Input::initialize(window.getWindow());

    // Initialize action system
    Action actionSystem;
    Physics physicsSystem;

    // int leftdir = -1;
    // int rightdir = 1;
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

        // Secondary object movement
        // miscMovement(objects, initialWorldWidth, leftdir, rightdir, actionSystem);

        // Apply physics step
        physicsSystem.playerMovementStep(player, deltaTime);
        // queueActions(objects, actionSystem, deltaTime);

        // Compute MVP, draw objects
        // drawStep(window, renderer, shader, objects);
        // tilemap.renderTileMap(shader, renderer);
        // drawStepPlayer(window, renderer, shader, player);
        // window.swap();

        drawTilemapAndPlayer(window, renderer, shader, tilemap, player);
        // Render tilemap

        // applyActions(objects, actionSystem);
        
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