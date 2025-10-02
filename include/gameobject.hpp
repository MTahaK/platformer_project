#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <memory>
#include "behavior.hpp"
struct AABB {
		float left, right, top, bottom;
};

enum class FacingDirection {
		LEFT,
		RIGHT
};

bool checkCollision(const AABB& a, const AABB& b);

class GameObject {

	public:
		// Default constructor
		GameObject();
		// Position + color (default scale = (1,1), rotation = 0)
		GameObject(const glm::vec2& position, const glm::vec4& color);
		// Full constructor
		GameObject(const glm::vec2& position, const glm::vec2& scale, float rotation, const glm::vec4& color);

		// Setters
		// After any transformation, AABB must be recomputed
		void setPosition(const glm::vec2& position) {
			position_ = position;
			computeAABB();
		}
		void offsetPosition(const glm::vec2& offset) {
			position_ += offset;
			computeAABB();
		}
		void multPosition(const glm::vec2& multiplier) {
			position_ *= multiplier;
			computeAABB();
		}

		void setInitPosition(const glm::vec2& initPos) { initPos_ = initPos; }

		void setScale(const glm::vec2& scale) {
			scale_ = scale;
			computeAABB();
		}
		void offsetScale(const glm::vec2& offset) {
			scale_ += offset;
			computeAABB();
		}
		void multScale(const glm::vec2& multiplier) {
			scale_ *= multiplier;
			computeAABB();
		}

		void setRotation(float rotation) {
			rotation_ = rotation;
			computeAABB();
		}
		void offsetRotation(float offset) {
			rotation_ += offset;
			computeAABB();
		}
		void multRotation(float multiplier) {
			rotation_ *= multiplier;
			computeAABB();
		}

		void setColor(const glm::vec4& color) { color_ = color; }
		void setName(const std::string& name) { name_ = name; }

		void setVelocity(const glm::vec2& velocity) { velocity_ = velocity; } // Set the velocity vector
		void addVelocity(const glm::vec2& delta) { velocity_ += delta; }	  // Add to the velocity vector

		void setAcceleration(const glm::vec2& acceleration) { acceleration_ = acceleration; } // Set the acceleration vector

		void setGrounded(bool grounded) { isGrounded_ = grounded; } // Set whether the object is grounded

		// Getters
		const glm::vec2& getPosition() const { return position_; }
		const glm::vec2& getScale() const { return scale_; }
		float getRotation() const { return rotation_; }

		const glm::vec2& getInitPosition() const { return initPos_; }

		const glm::vec4& getColor() const { return color_; }
		const std::string& getName() const { return name_; }

		const glm::vec2& getVelocity() const { return velocity_; } // Returns the velocity vector

		const glm::vec2& getAcceleration() const { return acceleration_; } // Returns the acceleration vector

		void applyVelocity(float deltaTime) {
			offsetPosition(velocity_ * deltaTime); // Apply velocity to position based on deltaTime
		}

		bool isGrounded() const { return isGrounded_; } // Returns whether the object is grounded

		// AABB-related methods
		const void computeAABB();
		const AABB computeOffsetAABB(const glm::vec2& offset); // Computes AABB based on next position
		// NOTE: This EXPLICITLY DOES NOT UPDATE THE INTERNAL AABB.
		const AABB& getAABB() const { return aabb_; } // Returns the computed AABB

		glm::mat4 getModelMatrix() const; // Computes and returns model matrix

		// Behavior management
		void setBehavior(std::unique_ptr<Behavior> behavior);
		void updateBehavior(float deltaTime);
		void handlePlayerCollision(class PlayerObject& player);
		Behavior* getBehavior() const { return behavior_.get(); }

		// Texture rendering and management
		FacingDirection getFacingDirection() const { return facingDirection_; }
		void setFacingDirection(FacingDirection dir) { facingDirection_ = dir; }
		void swapFacingDirection();

		Texture* getTexture() const { return texture_; }
		void setTexture(Texture* texture) { texture_ = texture; }
		void clearTexture() { texture_ = nullptr; }
		
	private:
		glm::vec2 position_; // X, Y position
		glm::vec2 scale_;	 // Scaling coefficient (in each direction)
		float rotation_;	 // Angle in radians

		glm::vec2 initPos_;

		glm::vec4 color_;
		std::string name_;

		glm::vec2 velocity_ = glm::vec2(0.0f);	   // Velocity vector
		glm::vec2 acceleration_ = glm::vec2(0.0f); // Acceleration vector (not used yet)
		AABB aabb_;								   // Axis-aligned bounding box for collision detection

		bool isGrounded_ = false;

		// Ok fine I'll make it private
		FacingDirection facingDirection_ = FacingDirection::RIGHT;

		// Pointer to texture
		Texture* texture_ = nullptr;

		// Behavior system
		std::unique_ptr<Behavior> behavior_;
};