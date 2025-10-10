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
	if(speedX > 0.1f){
		if(speedX < 6.0f){
			moveState_ = MoveState::WALK;
		}else if(speedX < 18.0f){
			moveState_ = MoveState::RUN1;
		}else{
			moveState_ = MoveState::RUN2;
		}
	}else{
		moveState_ = MoveState::IDLE;
	}
}
UVRect PlayerObject::gridCellUV(int col, int row, int cols, int rows) {
	const float cw = 1.0f / cols;           // normalized cell width
    const float ch = 1.0f / rows;           // normalized cell height

    float u0 = col * cw;
    float u1 = (col + 1) * cw;

    float v0, v1;

	v0 = 1.0f - (row + 1) * ch;
	v1 = 1.0f - row * ch;

    return {u0, v0, u1, v1};
}

void PlayerObject::initAnimation(){
	currentFrame = idleAnim.startIdx;
	UVRect rect = gridCellUV(currentFrame.x, currentFrame.y, numFramesX, numFramesY);
	uvMin = glm::vec2(rect.u0, rect.v0);
	uvMax = glm::vec2(rect.u1, rect.v1);
	if(getFacingDirection() == FacingDirection::RIGHT){
        std::swap(uvMin.x, uvMax.x);
    }
}

void PlayerObject::updateAnimation(float deltaTime, float frameDuration){
	frameTimer += deltaTime;
	SpriteAnim anim;

	switch(moveState_){
		case MoveState::IDLE:
			anim = idleAnim;
			break;
		case MoveState::WALK:
			anim = walkAnim;
			break;
		case MoveState::RUN1:
			anim = run1Anim;
			break;
		case MoveState::RUN2:
			anim = run2Anim;
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

	if(moveState_ != prevMoveState_){ 
        prevMoveState_ = moveState_;
        currentFrame = anim.startIdx;
        frameTimer = 0.0f;
    }

	// Sprite 'array' is a 8x7 grid, 8 cols, 7 rows
	// Bounds are defined in player object as numFramesX/Y
	if(frameTimer >= frameDuration){
		// Current frame has lingered for long enough, advance frame
		frameTimer -= frameDuration;

		if (currentFrame == anim.endIdx) {
			// If we've reached the end frame, loop back to start
			// DEBUG_ONLY(std::cout << "Looping animation back to start frame\n";);
			currentFrame = anim.startIdx;
		}
		// If we've gone past the end of this row, wrap to next row
        if(currentFrame.x > numFramesX - 1){
			currentFrame.x = 0;
            currentFrame.y++;
        }
		
		currentFrame.x++;
        // If we've gone past the last row, wrap back to start
		// Based on the setup of the sprite sheet, this is actually unnecessary
        // if(currentFrame.y > numFramesY - 1){
        //     currentFrame = startFrame; // Reset to first frame
        // }
	}

	UVRect rect = gridCellUV(currentFrame.x, currentFrame.y, numFramesX, numFramesY);
	uvMin = glm::vec2(rect.u0, rect.v0);
	uvMax = glm::vec2(rect.u1, rect.v1);


	if(getFacingDirection() == FacingDirection::RIGHT){
        std::swap(uvMin.x, uvMax.x);
    }
}