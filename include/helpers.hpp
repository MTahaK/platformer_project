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
#include "window.hpp"
#include "shader.hpp"
#include "renderer2d.hpp"
#include "gameobject.hpp"
#include "playerobject.hpp"
#include "input.hpp"
#include "action.hpp"
#include "physics.hpp"
#include "tilemap.hpp"
#include "debug.hpp"

constexpr double targetFPS = 120.0;
constexpr double targetFrameTime = 1.0 / targetFPS; // ~0.016666... seconds


int initializeVisuals(Shader& shader, Renderer2D& renderer);

// SETUP FUNCTIONS  
PlayerObject setupPlayerObject(Tilemap& tilemap, int tileX, int tileY);
std::vector<GameObject> setupObjects(float& worldHeight, float& worldWidth);

// INPUT HANDLING -  Will likely add more state-specific functions later, but may move.
int playerInput(GameObject& player);

// RENDERING FUNCTIONS - some not used
void drawStep(Window& window, Renderer2D& renderer, Shader& shader, const std::vector<GameObject>& objects);
void drawStepPlayer(Window& window, Renderer2D& renderer, Shader& shader, const PlayerObject& player);
void drawTilemapAndPlayer(Window& window, Renderer2D& renderer, Shader& shader, const Tilemap& tilemap, const PlayerObject& player);

// ACTION SYSTEM FUNCTIONS - CURRENTLY UNUSED
void miscMovement(std::vector<GameObject>& objects, float initialWorldWidth, int& leftdir, int& rightdir, Action& actionSystem);
void queueActions(std::vector<GameObject>& objects, Action& actionSystem, float deltaTime);
void applyActions(std::vector<GameObject>& objects, Action& actionSystem);