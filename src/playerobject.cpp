#include "playerobject.hpp"
#include "debug.hpp"
#include <iostream>

void PlayerObject::setUpSensors() {
	leftSensor_.position = getPosition() + glm::vec2(-getScale().x / 2.0f - EPSILON, 0.0f);
	rightSensor_.position = getPosition() + glm::vec2(getScale().x / 2.0f + EPSILON, 0.0f);
	topSensor_.position = getPosition() + glm::vec2(0.0f, getScale().y / 2.0f + EPSILON);
	bottomSensor_.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - EPSILON);

	// Colours for debug
	bottomSensor_.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	topSensor_.color = glm::vec4(0.0f, 0.8f, 1.0f, 1.0f);
	leftSensor_.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	rightSensor_.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

void PlayerObject::sensorUpdate() {
	leftSensor_.position = getPosition() + glm::vec2(-getScale().x / 2.0f - EPSILON, 0.0f);
	rightSensor_.position = getPosition() + glm::vec2(getScale().x / 2.0f + EPSILON, 0.0f);
	topSensor_.position = getPosition() + glm::vec2(0.0f, getScale().y / 2.0f + EPSILON);
	bottomSensor_.position = getPosition() + glm::vec2(0.0f, -getScale().y / 2.0f - EPSILON);
}

const glm::ivec2 PlayerObject::getPlayerTileIdx(Tilemap& tilemap) const { return tilemap.worldToTileIndex(getPosition()); }

bool PlayerObject::tileCollision(Tilemap& tilemap, const Sensor& sensor) {
	glm::ivec2 tileIndex = tilemap.worldToTileIndex(sensor.position);
	bool isSolid = tilemap.isSolidTile(tileIndex.x, tileIndex.y);
	return isSolid; // Check if the tile at the sensor's position is solid
					// return tilemap.isSolidTile(tileIndex.x, tileIndex.y);
}

bool PlayerObject::tileGoalCollision(Tilemap& tilemap, const Sensor& sensor) {
	glm::ivec2 tileIndex = tilemap.worldToTileIndex(sensor.position);
	bool isGoal = tilemap.isGoalTile(tileIndex.x, tileIndex.y);
	if (isGoal) {
		if (goalCount_ < 4)
			goalCount_++;
	} else if (goalCount_ > 0) {
		goalCount_--; // Decrease goal count if the player is not on a goal tile
	} else if (goalCount_ < 0) {
		goalCount_ = 0; // Ensure goal count does not go negative
	} else {
		goalCount_ = 0; // Reset goal count if the player is not on a goal tile
	}
	return isGoal; // Check if the tile at the sensor's position is a goal tile
}
