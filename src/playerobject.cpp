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

void PlayerObject::updateMoveState(){
	auto speedX = abs(getVelocity().x);
	if(speedX > EPSILON){
		if(speedX < 5.0f){
			moveState_ = MoveState::WALK;
		}else if(speedX < 10.0f){
			moveState_ = MoveState::RUN1;
		}else{
			moveState_ = MoveState::RUN2;
		}
	}else{
		moveState_ = MoveState::IDLE;
	}
}
UVRect PlayerObject::gridCellUV(int col, int row, int cols, int rows, int texW, int texH) {
	const float cw = 1.0f / cols;           // normalized cell width
    const float ch = 1.0f / rows;           // normalized cell height

    float u0 = col * cw;
    float u1 = (col + 1) * cw;

    float v0, v1;

	v0 = 1.0f - (row + 1) * ch;
	v1 = 1.0f - row * ch;

    // Optional half-texel inset to avoid bleeding with linear filtering/mips
    // const float padU = 0.5f / float(texW);
    // const float padV = 0.5f / float(texH);
    // u0 += padU;  v0 += padV;
    // u1 -= padU;  v1 -= padV;

    return {u0, v0, u1, v1};
}

void PlayerObject::updateAnimation(float deltaTime, float frameDuration){
	static float frameTimer = 0.0f;
	frameTimer += deltaTime;

	int totalFrames = 1;
	glm::ivec2 startFrame(0);
	glm::ivec2 endFrame(0);

	switch(moveState_){
		case MoveState::IDLE:
			totalFrames = numFramesIdle;
			startFrame = idleStart;
			endFrame = idleEnd;
			break;
		case MoveState::WALK:
			totalFrames = numFramesWalk;
			startFrame = walkStart;
			endFrame = walkEnd;
			break;
		case MoveState::RUN1:
			totalFrames = numFramesRun1;
			startFrame = run1Start;
			endFrame = run1End;
			break;
		case MoveState::RUN2:
			totalFrames = numFramesRun2;
			startFrame = run2Start;
			endFrame = run2End;
			break;
		case MoveState::JUMP:
			// Jumping animation not implemented yet
			break;
		case MoveState::FALL:
			// Falling animation not implemented yet
			break;
		default:
			break;
	}

	if(frameTimer >= frameDuration){
		frameTimer -= frameDuration; // Reset timer but keep overflow

		// Update horizontal frame
		u_offset_ += 1.0f / float(totalFrames);
		if(u_offset_ > (float(endFrame.x) + 1.0f) / float(numFramesX)){
			u_offset_ = float(startFrame.x) / float(numFramesX);
		}

		// Update vertical frame
		v_offset_ = float(startFrame.y) / float(numFramesY); // Just set to the starting row for now
	}
}