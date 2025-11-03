#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "userinterface.hpp"

const float EPSILON = 0.001f;

const float TILE_SIZE = 1.2f;
const glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0f);

const float quadVerts[] = {
	// x, y
	-0.5f, -0.5f, // Bottom-left
	0.5f,  -0.5f, // Bottom-right
	0.5f,  0.5f,  // Top-right
	-0.5f, 0.5f   // Top-left
};