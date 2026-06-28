#include <iostream>
#include "nlohmann/json.hpp"
#include "playerobject.hpp"
#include "debug.hpp"

using json = nlohmann::json;

void PlayerObject::setUpSensors() {
	// Default sensor scaling factors = 1.0 (no scaling)

	leftSensor_.position = getPosition() + glm::vec2(-getScale().x * horizSensorScale_ / 2.0f - EPSILON, 0.0f);
	rightSensor_.position = getPosition() + glm::vec2(getScale().x * horizSensorScale_ / 2.0f + EPSILON, 0.0f);
	topSensor_.position = getPosition() + glm::vec2(0.0f, getScale().y * vertSensorScale_ / 2.0f + EPSILON);
	bottomSensor_.position = getPosition() + glm::vec2(0.0f, -getScale().y * vertSensorScale_ / 2.0f - EPSILON);

	// Colours for debug
	bottomSensor_.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	topSensor_.color = glm::vec4(0.0f, 0.8f, 1.0f, 1.0f);
	leftSensor_.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	rightSensor_.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

void PlayerObject::setSensorScale(float horizScale, float vertScale) {
	horizSensorScale_ = horizScale;
	vertSensorScale_ = vertScale;
}

void PlayerObject::sensorUpdate() {
	leftSensor_.position = getPosition() + glm::vec2(-getScale().x * horizSensorScale_ / 2.0f - EPSILON, 0.0f);
	rightSensor_.position = getPosition() + glm::vec2(getScale().x * horizSensorScale_ / 2.0f + EPSILON, 0.0f);
	topSensor_.position = getPosition() + glm::vec2(0.0f, getScale().y * vertSensorScale_ / 2.0f + EPSILON);
	bottomSensor_.position = getPosition() + glm::vec2(0.0f, -getScale().y * vertSensorScale_ / 2.0f - EPSILON);
}

const glm::ivec2 PlayerObject::getPlayerTileIdx(Tilemap& tilemap) const { return tilemap.worldToTileIndex(getPosition()); }

bool PlayerObject::tileCollision(Tilemap& tilemap, const Sensor& sensor) {
	glm::ivec2 tileIndex = tilemap.worldToTileIndex(sensor.position);
	bool isSolid = tilemap.isSolidTile(tileIndex.x, tileIndex.y);
	return isSolid; // Check if the tile at the sensor's position is solid
					// return tilemap.isSolidTile(tileIndex.x, tileIndex.y);
}

int PlayerObject::loadSpriteAtlas(const std::string& atlasDataPath){
	// If no texture set, abort
	if(getTexture() == nullptr){
		DEBUG_ONLY(std::cerr << "Error: No texture set for sprite atlas. Aborting sprite load.\n";);
		return -1;
	}

	// Load and parse JSON atlas data
	std::ifstream atlasFile(atlasDataPath);
	if (!atlasFile.is_open()) {
		DEBUG_ONLY(std::cerr << "Error: Could not open sprite atlas data file: " << atlasDataPath << "\n";);
		return -1;
	}

	json atlasJson = json::parse(atlasFile);

	spriteAtlas_.atlasWidth = atlasJson["meta"]["size"]["w"];
	spriteAtlas_.atlasHeight = atlasJson["meta"]["size"]["h"];

	for(auto& [framePath, frameData] : atlasJson["frames"].items()){
		size_t pos = framePath.find('/'); // Frames in subfolders named after animation
		if(pos == std::string::npos){
			DEBUG_ONLY(std::cerr << "Warning: Invalid frame path: " << framePath << "\n";);
			continue;  // Skip this frame
		}
		// Individual frames are enumerated as frame_00, frame_01, etc - 2 digits ALWAYS
		std::string animName = framePath.substr(0, pos);

		AtlasFrame atlasFrame;
		atlasFrame.x = frameData["frame"]["x"];
		atlasFrame.y = frameData["frame"]["y"];
		atlasFrame.w = frameData["frame"]["w"];
		atlasFrame.h = frameData["frame"]["h"];

		spriteAtlas_.animations[animName].frames.push_back(atlasFrame);
	}
	return 0;
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

void PlayerObject::updateMoveState(){
	auto speedX = abs(getVelocity().x);
	if(speedX > 0.1f){
		if(speedX < 5.0f){
			moveState_ = MoveState::WALK;
		} else if(speedX < 10.0f){
			moveState_ = MoveState::FASTWALK;
		}else if(speedX < 14.0f){
			moveState_ = MoveState::RUN1;
		} else if(speedX < 28.0f){
			moveState_ = MoveState::RUN2;
		}else{
			moveState_ = MoveState::RUN3;
		}
	}else{
		moveState_ = MoveState::IDLE;
	}
}

UVRect PlayerObject::atlasFrameUV(const AtlasFrame& frame){
	float u0 = static_cast<float>(frame.x) / static_cast<float>(spriteAtlas_.atlasWidth);
	float u1 = static_cast<float>(frame.x + frame.w) / static_cast<float>(spriteAtlas_.atlasWidth);

	// OpenGL's v coords start from bottom left; flip
	float v0 = 1.0f - static_cast<float>(frame.y + frame.h) / static_cast<float>(spriteAtlas_.atlasHeight);
	float v1 = 1.0f - static_cast<float>(frame.y) / static_cast<float>(spriteAtlas_.atlasHeight);

	return {u0, v0, u1, v1};
}


void PlayerObject::initAtlasAnimation(){
	currentAtlasAnim_ = &spriteAtlas_.animations["idle"];
	UVRect rect = atlasFrameUV(currentAtlasAnim_->frames[0]);
	uvMin = glm::vec2(rect.u0, rect.v0);
	uvMax = glm::vec2(rect.u1, rect.v1);
	if(getFacingDirection() == FacingDirection::RIGHT){
		std::swap(uvMin.x, uvMax.x);
	}
}

void PlayerObject::updateAtlasAnimation(float deltaTime, float frameDuration){
	frameTimer_ += deltaTime;
	
	if(moveState_ != prevMoveState_){
		prevMoveState_ = moveState_;
		switch(moveState_){
			case MoveState::IDLE:
				currentAtlasAnim_ = &spriteAtlas_.animations["idle"];
				break;
			case MoveState::WALK:
				currentAtlasAnim_ = &spriteAtlas_.animations["walk"];
				break;
			case MoveState::FASTWALK:
				currentAtlasAnim_ = &spriteAtlas_.animations["fastWalk"];
				break;
			case MoveState::RUN1:
				currentAtlasAnim_ = &spriteAtlas_.animations["run1"];
				break;
			case MoveState::RUN2:
				currentAtlasAnim_ = &spriteAtlas_.animations["run2"];
				break;
			case MoveState::RUN3:
				currentAtlasAnim_ = &spriteAtlas_.animations["run3"];
				break;
			default:
				currentAtlasAnim_ = &spriteAtlas_.animations["idle"];
				break; 
		}
		frameTimer_ = 0.0f;
		currentAtlasAnim_->currentFrameIdx = 0;

		// Resize quad if needed based on frame size
		
	} else{
		// Continue current animation
		if(frameTimer_ >= frameDuration){
			frameTimer_ -= frameDuration;
			currentAtlasAnim_->currentFrameIdx++;
			if(currentAtlasAnim_->currentFrameIdx >= currentAtlasAnim_->frames.size() && currentAtlasAnim_->loop){
				currentAtlasAnim_->currentFrameIdx = 0; // Loop back to start
			}
		}
	}

	UVRect rect = atlasFrameUV(currentAtlasAnim_->frames[currentAtlasAnim_->currentFrameIdx]);
	uvMin = glm::vec2(rect.u0, rect.v0);
	uvMax = glm::vec2(rect.u1, rect.v1);

	if(getFacingDirection() == FacingDirection::RIGHT){
        std::swap(uvMin.x, uvMax.x);
    }

}