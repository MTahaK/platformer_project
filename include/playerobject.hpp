#pragma once
#include <glm/glm.hpp>
#include <string>
#include "gameobject.hpp"
#include "tilemap.hpp"
#include "globals.hpp"
#include "texture.hpp"
struct Sensor {
		glm::vec2 position; // Position of the sensor relative to the player
		glm::vec4 color;	// Only used for debug view
};

struct SpriteAnim {
	std::string animName;
	glm::ivec2 startIdx;
	glm::ivec2 endIdx;
	
};
struct AtlasFrame {
	int x, y, w, h;  // Rectangle in atlas (pixels)
};
struct AtlasAnimation {
	// std::string name;
	std::vector<AtlasFrame> frames;
	int currentFrameIdx = 0;
	bool loop = true;
	float fps = 8.0f;
};

struct SpriteAtlas {
	// Distinct from atlas texture, refers to atlas metadata
	int atlasWidth;  // Width of the atlas in pixels
	int atlasHeight; // Height of the atlas in pixels
	std::unordered_map<std::string, AtlasAnimation> animations;
	int baseFrameWidth = 0;   // Reference frame width for scaling
	int baseFrameHeight = 0;  // Reference frame height for scaling
};


enum class MoveState { IDLE, WALK, FASTWALK, RUN1, RUN2, RUN3, JUMP, FALL};

const std::vector<std::string> playerAnims = {
	"idle",
	"walk",
	"fastWalk",
	"run1",
	"run2",
	"run3",
};
struct UVRect {
		float u0, v0, u1, v1;
}; // min/max, normalized

// PlayerObject class inherits from GameObject
class PlayerObject : public GameObject {

	public:
		void setUpSensors();
		void setSensorScale(float horizScale, float vertScale);
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

		int loadSpriteAtlas(const std::string& atlasDataPath);

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

		SpriteAnim walkAnim = {"WALK", glm::ivec2(0,0), glm::ivec2(7,0)};
		SpriteAnim run1Anim = {"RUN1", glm::ivec2(0,1), glm::ivec2(7,1)};
		SpriteAnim run2Anim = {"RUN2", glm::ivec2(0,2), glm::ivec2(7,2)};
		SpriteAnim idleAnim = {"IDLE", glm::ivec2(0,3), glm::ivec2(6,6)};

		inline UVRect gridCellUV(int col, int row, int cols, int rows);
		inline UVRect atlasFrameUV(const AtlasFrame& frame);
		void initAnimation();
		void initAtlasAnimation();
		
		void updateAtlasAnimation(float deltaTime, float frameDuration);
		void updateAnimation(float deltaTime, float frameDuration);

		float frameTimer_ = 0.0f;
		glm::ivec2 currentFrame_ = glm::ivec2(0, 0);

		AtlasFrame currentAtlasFrame_ = {0,0,0,0};
		AtlasAnimation* currentAtlasAnim_ = nullptr;

		// Sensor scaling factors assume default player scale of (1,1)
		float horizSensorScale_ = 1.0f;
		float vertSensorScale_ = 1.0f;

		glm::vec2 baseScale_ = glm::vec2(1.0f, 1.0f); // Store the original scale for proportional sizing

	private:
		// PlayerObject adds four 'sensors' to detect collisions

		Sensor leftSensor_;	  // Left sensor for detecting walls
		Sensor rightSensor_;  // Right sensor for detecting walls
		Sensor topSensor_;	  // Top sensor for detecting ceilings
		Sensor bottomSensor_; // Bottom sensor for detecting floors

		int goalCount_ = 0; // Counter for the number of sensors that detect a goal tile
		bool inGoal_ = false;
		bool shouldDie_ = false; // Processed during player update

		SpriteAtlas spriteAtlas_;
};