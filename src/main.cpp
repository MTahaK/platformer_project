#include "helpers.hpp"
#include "gamemanager.hpp"

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

    // Set player scale to (0.6f, 1.0f)
    // player.setScale(glm::vec2(0.6f, 1.0f));

    player.setGrounded(true); // Set the player object to be grounded initially

    Input::initialize(window.getWindow());

    Physics physicsSystem;

    GameManager gameManager(window, shader, renderer, player, tilemap, physicsSystem);
        
    while(!window.shouldClose()){
        gameManager.runGameLoop();
    }
    // Cleanup and exit
    renderer.shutdown();
    shader.shutdown();
    window.destroy();
    std::cout << "Exiting application." << std::endl;
    return 0;
}