#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
struct AABB{
    float left, right, top, bottom;
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
        void setPosition(const glm::vec2& position) { position_ = position; computeAABB();}
        void offsetPosition(const glm::vec2& offset) { position_ += offset; computeAABB();}
        void multPosition(const glm::vec2& multiplier) { position_ *= multiplier; computeAABB();}

        void setScale(const glm::vec2& scale) { scale_ = scale; computeAABB();}
        void offsetScale(const glm::vec2& offset) { scale_ += offset; computeAABB();}
        void multScale(const glm::vec2& multiplier) { scale_ *= multiplier; computeAABB();}

        void setRotation(float rotation) { rotation_ = rotation; computeAABB();}
        void offsetRotation(float offset) { rotation_ += offset; computeAABB();}
        void multRotation(float multiplier) { rotation_ *= multiplier; computeAABB();}
        
        void setColor(const glm::vec4& color) { color_ = color; }
        void setName(const std::string& name) { name_ = name; }

        // Getters
        const glm::vec2& getPosition() const { return position_; }
        const glm::vec2& getScale() const { return scale_; }
        float getRotation() const { return rotation_; }
        const glm::vec4& getColor() const { return color_; }
        const std::string& getName() const { return name_; }


        const void computeAABB();
        const AABB computeOffsetAABB(const glm::vec2& offset); // Computes AABB based on next position
        // NOTE: This EXPLICITLY DOES NOT UPDATE THE INTERNAL AABB.
        const AABB& getAABB() const { return aabb_; } // Returns the computed AABB
        
        glm::mat4 getModelMatrix() const; // Computes and returns model matrix

    private:
        glm::vec2 position_;    // X, Y position
        glm::vec2 scale_;       // Scaling coefficient (in each direction)
        float rotation_;        // Angle in radians
        glm::vec4 color_;
        std::string name_;
        AABB aabb_;             // Axis-aligned bounding box for collision detection
};