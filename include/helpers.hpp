#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>
#include <memory>
#include <functional>
#include "window.hpp"
#include "shader.hpp"
#include "renderer2d.hpp"
#include "renderer3d.hpp"
#include "gameobject.hpp"
#include "playerobject.hpp"
#include "input.hpp"
#include "physics.hpp"
#include "tilemap.hpp"
#include "debug.hpp"
#include "texture.hpp"
#include "gamemanager.hpp"
#include "globals.hpp"
#include "userinterface.hpp"
#include "fonts.hpp"

constexpr double targetFPS = 120.0;
constexpr double targetFrameTime = 1.0 / targetFPS; // ~0.016666... seconds

enum class InputResult {
	CONTINUE = 0,
	PAUSE = 1,
	FORCE_QUIT = 2 // In normal play, quitting must be performed from PAUSE
};

int initializeVisuals(Shader& shader, Renderer2D& renderer);

// SETUP FUNCTIONS
PlayerObject setupPlayerObject(Tilemap& tilemap, int tileX, int tileY);
// std::vector<GameObject> setupObjects(float& worldHeight, float& worldWidth);

// INPUT HANDLING -  Will likely add more state-specific functions later, but may move.
InputResult playerInput(GameObject& player);

// RENDERING FUNCTIONS - some not used
void drawStep(Window& window, Renderer2D& renderer, Shader& shader, const std::vector<GameObject>& objects);
void drawStepPlayer(Window& window, Renderer2D& renderer, Shader& shader, const PlayerObject& player);
void drawTilemapAndPlayer(Window& window, Renderer2D& renderer, Shader& shader, const Tilemap& tilemap, const PlayerObject& player);
void drawObjects(Window& window, Renderer2D& renderer, Shader& shader, const std::vector<GameObject>& objects);
void finishDraw(Window& window, Renderer2D& renderer, Shader& shader);
void finishDraw3D(Window& window, Renderer3D& renderer, Shader& shader);
void renderCountdown(float countdownTime);

// UPDATE FUNCTIONS
void updatePStatePlayer(PlayerObject& player, Physics& physics, Tilemap& tilemap, std::vector<GameObject>& objects, float deltaTime);
void updateDeathWall(GameObject& deathWall, float deltaTime);

// UTILITY FUNCTIONS
std::string currentShapeToString(CurrentShape shape);