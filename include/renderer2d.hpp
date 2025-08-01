#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "shader.hpp"
#include <string>
#include "globals.hpp"

struct BatchVertex {
	glm::vec2 position;
	glm::vec4 color;
	// Once textures are implemented, will include UV coords
};

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

		void flushBatch();
		void addQuadtoBatch(const glm::mat4 transform, const glm::vec4& color);
		
	private:

		static const uint32_t MAX_QUADS = 10000;
		static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
		static const uint32_t MAX_INDICES = MAX_QUADS * 6;

		std::vector<BatchVertex> batchVertices_;
		std::vector<uint32_t> batchIndices_;
		uint32_t vertexCount_ = 0; // Number of vertices currently in use
		uint32_t indexCount_ = 0; // Number of indices currently in use

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

