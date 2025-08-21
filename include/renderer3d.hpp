#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "shader.hpp"
#include "globals.hpp"

struct Vertex3D {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 normal;
};

enum class CurrentShape {
	NONE,
	TRIANGLE,
	PLANE,
	CUBE,
	PYRAMID,
	SPHERE
};
class Renderer3D {

	public:
		Renderer3D() = default;
		~Renderer3D();

		bool init(Shader& shader);
		void shutdown();

		void beginScene(const glm::mat4& view, const glm::mat4& proj);
		void setShader(Shader& shader);
		void setLightingUniforms(const glm::vec3& lightPos, const glm::vec3& viewPos, const glm::vec3& lightColor);
		void drawTriangle(const glm::mat4& transform);
		void drawPlane(const glm::mat4& transform);
		void drawCube(const glm::mat4& transform);
		void drawLightCube(const glm::mat4& transform, const glm::vec4& color);
		void drawPyramid(const glm::mat4& transform);
		void drawSphere(const glm::mat4& transform);

		// void drawTriangleColor(const glm::mat4& transform, const glm::vec4& color);
		// void drawPlaneColor(const glm::mat4& transform, const glm::vec4& color);
		// void drawCubeColor(const glm::mat4& transform, const glm::vec4& color);
		// void drawPyramidColor(const glm::mat4& transform, const glm::vec4& color);
		// void drawSphereColor(const glm::mat4& transform, const glm::vec4& color);
		void setCurrentShape(CurrentShape shape);

		CurrentShape getCurrentShape() const;

	private:
		std::vector<Vertex3D> triangleVertices_;
		std::vector<unsigned int> triangleIndices_;

		std::vector<Vertex3D> planeVertices_;
		std::vector<unsigned int> planeIndices_;

		std::vector<Vertex3D> cubeVertices_;
		std::vector<unsigned int> cubeIndices_;

		std::vector<Vertex3D> lightCubeVertices_;
		std::vector<unsigned int> lightCubeIndices_;

		std::vector<Vertex3D> pyramidVertices_;
		std::vector<unsigned int> pyramidIndices_;

		std::vector<Vertex3D> sphereVertices_;
		std::vector<unsigned int> sphereIndices_;

		GLuint shader_ = 0;
		Shader* currentShader_ = nullptr;  // Keep reference to current shader
		GLuint vao_;
		// GLuint vbo_;
		// GLuint ebo_;

		
		GLuint triangleVBO_;
		GLuint triangleEBO_;
		
		GLuint planeVBO_;
		GLuint planeEBO_;
		
		GLuint cubeVBO_;
		GLuint cubeEBO_;
		
		GLuint lightCubeVBO_;
		GLuint lightCubeEBO_;

		GLuint pyramidVBO_;
		GLuint pyramidEBO_;

		GLuint sphereVBO_;
		GLuint sphereEBO_;

		glm::mat4 model_;
		glm::mat4 view_;
		glm::mat4 proj_;

		CurrentShape currentShape_ = CurrentShape::NONE;
		bool shaderLoaded_ = false;
};