#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"
#include <string>

// Renderer2D class for 2D rendering operations
class Renderer2D {

    public:
        Renderer2D() = default;
        ~Renderer2D();
        bool init(Shader& shader);
        bool initLine(Shader& shader); // For line rendering
        void shutdown();
        void beginScene(Shader& shader, const glm::mat4& view, const glm::mat4& proj); // Runs at start of frame before drawing
        void drawQuad(Shader& shader, const glm::mat4& transform, const glm::vec4& color);
        void drawLine(Shader& shader, const glm::vec2& start, const glm::vec2& end, const glm::vec4& color);
        void endScene();  // Runs at end of frame after drawing
        // Doesn't really do anything now, as I'm using immediate rendering

    private:

        GLuint shader_ = 0;
        GLuint vao_;
        GLuint vbo_;
        GLuint ebo_;

        GLuint lineVAO_;
        GLuint lineVBO_;

        glm::mat4 model_;
        glm::mat4 view_;
        glm::mat4 proj_;

        bool shaderLoaded_ = false;
};

