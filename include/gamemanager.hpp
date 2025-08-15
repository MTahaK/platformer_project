#pragma once
#include <glm/glm.hpp>
#include <chrono>
#include <iostream>
#include "tilemap.hpp"
#include "physics.hpp"
#include "renderer2d.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "input.hpp"
#include "playerobject.hpp"
#include "debug.hpp"
#include "globals.hpp"
#include "color.hpp"

enum class GameState {
	MENU,
	// LOAD, may not used
	PLAY,
	PAUSE,
	DEAD,
	WIN,
	UIDEMO,
	EXIT
};

class GameManager {

	public:
		// Default constructor leaves everything uninitialized
		GameManager() = default;
		// Destructor
		~GameManager() = default;

		// Full initialization constructor
		GameManager(Window& window, Shader& shader, Renderer2D& renderer, PlayerObject& player, Tilemap& tilemap,
					std::vector<GameObject>& objects, Physics& physics);

		void runGameLoop();
		void setState(GameState state);

		GameState getState() const { return gameState_; }

		// State-specific routines, includes input handling, physics, rendering
		// for each state
		void handleMenuState();
		void handlePlayState();
		void handlePauseState();
		void handleDeadState();
		void handleWinState();
		void handleUIDemo();
		void handleExitState();

		// Subsystem returns
		PlayerObject& getPlayer() { return player_; }
		Tilemap& getTilemap() { return tilemap_; }
		std::vector<GameObject>& getObjects() { return objects_; }
		Physics& getPhysics() { return physics_; }
		Renderer2D& getRenderer() { return renderer_; }
		Shader& getShader() { return shader_; }

	private:
		GameState gameState_ = GameState::MENU;
		Window& window_;
		Shader& shader_;
		Renderer2D& renderer_;
		PlayerObject& player_;
		Tilemap& tilemap_;
		std::vector<GameObject>& objects_;
		Physics& physics_;

		// Timing management for game loop
		float lastFrameTime_ = 0.0f;
		float winTimer_ = 0.0f; // Timer for WIN state countdown
};