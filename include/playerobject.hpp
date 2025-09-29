#pragma once
#include <glm/glm.hpp>
#include "gameobject.hpp"
#include "tilemap.hpp"
#include "globals.hpp"
#include "texture.hpp"
struct Sensor {
		glm::vec2 position; // Position of the sensor relative to the player
		glm::vec4 color;	// Only used for debug view
};

enum class FacingDirection {
		LEFT,
		RIGHT
};

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

		// I don't feel like making this private
		FacingDirection facingDirection_ = FacingDirection::RIGHT;

		// Pointer to texture
		Texture* texture_ = nullptr;

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