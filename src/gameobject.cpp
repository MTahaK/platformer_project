#include "gameobject.hpp"

GameObject::GameObject() {
    // Initialize with default values
    position_ = glm::vec2(0.0f, 0.0f);
    scale_ = glm::vec2(1.0f, 1.0f);
    rotation_ = 0.0f;
    color_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Default white color
    name_ = "Unnamed";
}

GameObject::GameObject(const glm::vec2& position, const glm::vec4& color){
    // Initialize with just position and color - convenient for simple objects
    position_ = position;
    scale_ = glm::vec2(1.0f, 1.0f); // Default scale
    rotation_ = 0.0f; // Default rotation
    color_ = color;
    name_ = "Unnamed";
}

GameObject::GameObject(const glm::vec2& position, const glm::vec2& scale, float rotation, const glm::vec4& color) {
    // Full constructor
    position_ = position;
    scale_ = scale;
    rotation_ = rotation;
    color_ = color;
    name_ = "Unnamed";
}