#pragma once
#include <glm/glm.hpp>
#include "gameobject.hpp"
#include "tilemap.hpp"
#include "globals.hpp"
struct Sensor {
		glm::vec2 position; // Position of the sensor relative to the player
		glm::vec4 color;	// Only used for debug view
};

enum class MoveState { IDLE, WALK, RUN1, RUN2, JUMP, FALL };

struct UVRect {
		float u0, v0, u1, v1;
}; // min/max, normalized

// PlayerObject class inherits from GameObject
class PlayerObject : public GameObject {

	public:
		void setUpSensors();
		void sensorUpdate();

		void setGoalCount(int count) { goalCount_ = count; }
		void setInGoal(bool status) { inGoal_ = status; }
		void setShouldDie(bool status) { shouldDie_ = status; }
		void incrementGoalCount() { goalCount_++; }

		bool tileCollision(Tilemap& tilemap, const Sensor& sensor);
		bool tileGoalCollision(Tilemap& tilemap, const Sensor& sensor);

		const glm::ivec2 getPlayerTileIdx(Tilemap& tilemap) const;
		int getGoalCount() const { return goalCount_; }

		const Sensor& getLeftSensor() const { return leftSensor_; }
		const Sensor& getRightSensor() const { return rightSensor_; }
		const Sensor& getTopSensor() const { return topSensor_; }
		const Sensor& getBottomSensor() const { return bottomSensor_; }

		bool checkIfInGoal() { return inGoal_; }
		bool getShouldDie() const { return shouldDie_; }

		glm::vec2 uvMin = glm::vec2(0.0f, 0.0f); // UV coordinates for texture mapping
		glm::vec2 uvMax = glm::vec2(1.0f, 1.0f);

		void flipUVY() {
			// Not a real flip, essentially sets UV to 'right facing' according to texture
			uvMin.y = 1.0f;
			uvMax.y = 0.0f;
		}
		void flipUVX() {
			uvMin.x = 1.0f;
			uvMax.x = 0.0f;
		}

		MoveState moveState_ = MoveState::IDLE;
		MoveState prevMoveState_ = MoveState::IDLE;

		void updateMoveState();

		int numFramesX = 8;
		int numFramesY = 7;

		// starting and ending 'indices' as pairs for each animation in sprite sheet
		glm::ivec2 walkStart = glm::ivec2(0, 0);
		glm::ivec2 walkEnd = glm::ivec2(7, 0);

		glm::ivec2 run1Start = glm::ivec2(0, 1);
		glm::ivec2 run1End = glm::ivec2(7, 1);

		glm::ivec2 run2Start = glm::ivec2(0, 2);
		glm::ivec2 run2End = glm::ivec2(7, 2);

		glm::ivec2 idleStart = glm::ivec2(0, 3);
		glm::ivec2 idleEnd = glm::ivec2(6, 6);

		inline UVRect gridCellUV(int col, int row, int cols, int rows);
		void initAnimation();
		void updateAnimation(float deltaTime, float frameDuration);

		float frameTimer = 0.0f;
		glm::ivec2 currentFrame = glm::ivec2(0, 0);

	private:
		// PlayerObject adds four 'sensors' to detect collisions

		Sensor leftSensor_;	  // Left sensor for detecting walls
		Sensor rightSensor_;  // Right sensor for detecting walls
		Sensor topSensor_;	  // Top sensor for detecting ceilings
		Sensor bottomSensor_; // Bottom sensor for detecting floors

		int goalCount_ = 0; // Counter for the number of sensors that detect a goal tile
		bool inGoal_ = false;
		bool shouldDie_ = false; // Processed during player update
};