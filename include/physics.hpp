#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include "globals.hpp"
#include "gameobject.hpp"
#include "playerobject.hpp"
#include "behavior.hpp"
#include "tilemap.hpp"
#include "debug.hpp"

const float gravity = -8.0f;
const float MAX_VELOCITY = 35.0f; // Maximum velocity limit

const float movementAccel = 4.0f;  // Acceleration applied when moving left/right
const float slowdownAccel = 25.0f; // Acceleration applied when slowing down
const float turnaroundAccel = 50.0f;
const float midairDrag = 5.0f;

class Physics {

	public:
		void playerMovementStep(PlayerObject& player, float deltaTime);
		void checkPlayerWorldCollisions(PlayerObject& player, Tilemap& tilemap);
		void checkPlayerDeathWallCollision(PlayerObject& player, GameObject& deathWall);
		void checkPlayerEntityCollisions(GameObject& obj, const std::vector<GameObject>& entities);

		float deltaTime = 0.0f;
};