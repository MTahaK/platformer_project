#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>


class GameObject {

    public:
        // Default constructor
        GameObject();
        // Position + color (default scale = (1,1), rotation = 0)
        GameObject(const glm::vec2& position, const glm::vec4& color);
        // Full constructor
        GameObject(const glm::vec2& position, const glm::vec2& scale, float rotation, const glm::vec4& color);

        // Setters
        void setPosition(const glm::vec2& position) { position_ = position; }
        void setScale(const glm::vec2& scale) { scale_ = scale; }
        void setRotation(float rotation) { rotation_ = rotation; }
        void setColor(const glm::vec4& color) { color_ = color; }
        void setName(const std::string& name) { name_ = name; }

        // Getters
        const glm::vec2& getPosition() const { return position_; }
        const glm::vec2& getScale() const { return scale_; }
        float getRotation() const { return rotation_; }
        const glm::vec4& getColor() const { return color_; }
        const std::string& getName() const { return name_; }

        glm::mat4 getModelMatrix() const; // Computes and returns model matrix

    private:
        glm::vec2 position_;    // X, Y position
        glm::vec2 scale_;       // Scaling coefficient (in each direction)
        float rotation_;        // Angle in radians
        glm::vec4 color_;
        std::string name_;       
};