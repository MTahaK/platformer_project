#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include "globals.hpp"
#include "gameobject.hpp"
#include "playerobject.hpp"
#include "tilemap.hpp"
#include "debug.hpp"

const float gravity = -8.0f;
const float MAX_VELOCITY = 15.0f; // Maximum velocity limit

const float movementAccel = 7.0f; // Acceleration applied when moving left/right
const float slowdownAccel = 16.0f; // Acceleration applied when slowing down
const float turnaroundAccel = 36.0f;
const float midairDrag = 1.0f;

class Physics {

	public:

	void playerMovementStep(PlayerObject& player, float deltaTime);
	void checkPlayerWorldCollisions(PlayerObject& player, Tilemap& tilemap);
	void checkPlayerEntityCollisions(GameObject& obj, const std::vector<GameObject>& entities); // May end up unused

	float deltaTime = 0.0f;

};