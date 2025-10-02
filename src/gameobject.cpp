#include "gameobject.hpp"
#include "behavior.hpp"
#include "playerobject.hpp"
#include <iostream>

GameObject::GameObject() {
	// Initialize with default values
	position_ = glm::vec2(0.0f, 0.0f);
	scale_ = glm::vec2(1.0f, 1.0f);
	rotation_ = 0.0f;
	color_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Default white color
	name_ = "Unnamed";
	computeAABB(); // Compute AABB based on default position and scale
}

GameObject::GameObject(const glm::vec2& position, const glm::vec4& color) {
	// Initialize with just position and color - convenient for simple objects
	position_ = position;
	scale_ = glm::vec2(1.0f, 1.0f); // Default scale
	rotation_ = 0.0f;				// Default rotation
	color_ = color;
	name_ = "Unnamed";
	computeAABB(); // Compute AABB based on position and scale
				   // Note: AABB will be computed with default scale (1,1) and rotation
}

GameObject::GameObject(const glm::vec2& position, const glm::vec2& scale, float rotation, const glm::vec4& color) {
	// Full constructor
	position_ = position;
	scale_ = scale;
	rotation_ = rotation;
	color_ = color;
	name_ = "Unnamed";
	computeAABB(); // Compute AABB based on position, scale,
}

glm::mat4 GameObject::getModelMatrix() const {
	// Compute the model matrix based on position, scale, and rotation
	glm::mat4 model = glm::mat4(1.0f);									// Start with identity matrix
	model = glm::translate(model, glm::vec3(position_, 0.0f));			// Translate to position
	model = glm::rotate(model, -rotation_, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z-axis
	model = glm::scale(model, glm::vec3(scale_, 1.0f));					// Scale in X and Y directions
	return model;
}

const void GameObject::computeAABB() {
	// Compute the Axis-Aligned Bounding Box (AABB) based on position, scale, and rotation
	float halfWidth = scale_.x / 2.0f;
	float halfHeight = scale_.y / 2.0f;

	aabb_.left = position_.x - halfWidth;
	aabb_.right = position_.x + halfWidth;
	aabb_.top = position_.y + halfHeight;
	aabb_.bottom = position_.y - halfHeight;
}

bool checkCollision(const AABB& a, const AABB& b) {
	bool collision;
	if (a.left < b.right && a.right > b.left && a.top > b.bottom && a.bottom < b.top) {
		collision = true;
	} else {
		collision = false;
	}
	return (collision);
}

const AABB GameObject::computeOffsetAABB(const glm::vec2& offset) {
	// Computes AABB based on next position after applying offset
	AABB offsetAABB;
	float halfWidth = scale_.x / 2.0f;
	float halfHeight = scale_.y / 2.0f;

	offsetAABB.left = (position_.x + offset.x) - halfWidth;
	offsetAABB.right = (position_.x + offset.x) + halfWidth;
	offsetAABB.top = (position_.y + offset.y) + halfHeight;
	offsetAABB.bottom = (position_.y + offset.y) - halfHeight;

	return offsetAABB;
}

// Behavior management implementations
void GameObject::setBehavior(std::unique_ptr<Behavior> behavior) { behavior_ = std::move(behavior); }

void GameObject::updateBehavior(float deltaTime) {
	if (behavior_) {
		behavior_->update(*this, deltaTime);
	}
}

void GameObject::handlePlayerCollision(PlayerObject& player) {
	if (behavior_) {
		behavior_->onPlayerCollision(*this, player);
	}
}

void GameObject::swapFacingDirection() {
	facingDirection_ = (facingDirection_ == FacingDirection::LEFT) ? FacingDirection::RIGHT : FacingDirection::LEFT;
}
