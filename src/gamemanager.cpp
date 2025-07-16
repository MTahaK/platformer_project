#include "gamemanager.hpp"

GameManager::GameManager(Window& window, Shader& shader, Renderer2D& renderer, PlayerObject& player, Tilemap& tilemap, Physics& physics): 
                        window_(window), shader_(shader), renderer_(renderer), player_(player), tilemap_(tilemap), physics_(physics) {
}

void GameManager::runGameLoop() {
    switch (gameState_) {
        case GameState::MENU:
            handleMenuState();
            break;
        case GameState::PLAY:
            handlePlayState();
            break;
        case GameState::PAUSE:
            handlePauseState();
            break;
        case GameState::DEAD:
            handleDeadState();
            break;
        case GameState::WIN:
            handleWinState();
            break;
        case GameState::EXIT:
            handleExitState();
            break;
    }
}

void GameManager::handleMenuState() {

    window_.pollEvents();
    // Currently, render a single quad that matches the framebuffer size
    int fbWidth, fbHeight;
    window_.getFramebufferSize(fbWidth, fbHeight);

    // Define projection matrix to match the framebuffer size

    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(fbWidth),
        0.0f, static_cast<float>(fbHeight),
        -1.0f, 1.0f
    );

    // View matrix: identity, fixed in place
    glm::mat4 view = glm::mat4(1.0f);
    renderer_.beginScene(shader_, view, projection); // Begin the scene

    // Compute model matrix for screen-size quad
    glm::mat4 model = glm::mat4(1.0f);
    // Center the quad in the framebuffer
    model = glm::translate(model, glm::vec3(fbWidth / 2.0f, fbHeight / 2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(fbWidth, fbHeight, 1.0f)); // Scale to framebuffer size
    auto color = hexToVec4("#b86532");

    renderer_.drawQuad(shader_, model, color);

    window_.swap();

    Input::update();
    // Check for input to switch to PLAY state
    if (Input::isKeyPressed(GLFW_KEY_ENTER)) {
        setState(GameState::PLAY);
        std::cout<< "Switching to PLAY state." << std::endl;
    }
}

// All other state handlers currently empty, to be implemented later
void GameManager::handlePlayState(){};
void GameManager::handlePauseState(){};
void GameManager::handleDeadState(){};
void GameManager::handleWinState(){};
void GameManager::handleExitState(){};