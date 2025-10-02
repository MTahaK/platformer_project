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
const float MAX_VELOCITY = 15.0f; // Maximum velocity limit

const float movementAccel = 7.0f;  // Acceleration applied when moving left/right
const float slowdownAccel = 18.0f; // Acceleration applied when slowing down
const float turnaroundAccel = 40.0f;
const float midairDrag = 1.0f;

class Physics {

	public:
		void playerMovementStep(PlayerObject& player, float deltaTime);
		void checkPlayerWorldCollisions(PlayerObject& player, Tilemap& tilemap);
		void checkPlayerDeathWallCollision(PlayerObject& player, GameObject& deathWall);
		void checkPlayerEntityCollisions(GameObject& obj, const std::vector<GameObject>& entities);

		float deltaTime = 0.0f;
};