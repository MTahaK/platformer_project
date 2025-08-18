#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "shader.hpp"

struct Vertex3D {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 normal;
};

class Renderer3D {

	public:
		Renderer3D() = default;
		~Renderer3D();

		bool init(Shader& shader);
		void shutdown();

		void beginScene(Shader& shader, const glm::mat4& view, const glm::mat4& proj);
		void drawPlane(Shader& shader, const glm::mat4& transform, const glm::vec4& color);
		void drawCube(Shader& shader, const glm::mat4& transform, const glm::vec4& color);
		void drawPyramid(Shader& shader, const glm::mat4& transform, const glm::vec4& color);

	private:
		GLuint shader_ = 0;
		GLuint vao_;
		GLuint vbo_;
		GLuint ebo_;

		glm::mat4 model_;
		glm::mat4 view_;
		glm::mat4 proj_;

};