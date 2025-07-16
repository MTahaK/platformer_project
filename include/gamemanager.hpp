#pragma once
#include <glm/glm.hpp>
#include "tilemap.hpp"
#include "physics.hpp"
#include "renderer2d.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "input.hpp"
#include "playerobject.hpp"

enum class GameState {
    MENU,
    // LOAD, may not use
    PLAY,
    PAUSE,
    DEAD,
    WIN,
    EXIT
};

class GameManager {
    
    public:
        // Default constructor leaves everything uninitialized
        GameManager() = default;

        // Full initialization constructor
        GameManager(PlayerObject player, Tilemap tilemap, Physics physics, Renderer2D renderer, Shader shader);

        void runGameLoop();
        void setState();
        
        GameState getState();

        // State-specific routines, includes input handling, physics, rendering for each state
        void handleMenuState();
        void handlePlayState();
        void handlePauseState();
        void handleDeadState();
        void handleWinState();
        void handleExitState();

    private:
        GameState gameState_ = GameState::MENU;
        PlayerObject player_;
        Tilemap tilemap_;
        Physics physics_;
        Renderer2D renderer_;
        Shader shader_;
};