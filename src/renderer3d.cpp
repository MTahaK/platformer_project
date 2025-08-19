#include "renderer3d.hpp"
#include "shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Renderer3D::~Renderer3D() { shutdown(); }

bool Renderer3D::init(Shader& shader) {
	if (!shader.getID()) {
		std::cerr << "[Renderer2D] Shader program ID is 0, shader not loaded." << std::endl;
		return false; // Shader not loaded
	}
	shaderLoaded_ = true;
	shader_ = shader.getID();

	// Set up vertex array object (VAO), vertex buffer object (VBO), and element buffer object (EBO)
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);

	glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

	// Set up triangle geometry
	// The layout for this will be the same for any shape.
	triangleVertices_ = {
		// Triangle vertices (CCW winding when viewed from +Z)
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // Bottom-left, red
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  // Bottom-right, green
		{{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	  // Top-center, blue
	};

	triangleIndices_ = {
		0, 1, 2 // CCW winding for front-facing triangle
	};

	planeVertices_ = {
		{{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},   // Top right, red
		{{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  // Top left, green
		{{-0.5f, -0.5f, 0.0f}, {0.5f, 0.0f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // Bottom left, purple
		{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  // Bottom right, blue
	};

	planeIndices_ = {
		0, 1, 2,
		2, 3, 0
	};
	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey
	currentShape_ = CurrentShape::NONE;
	
	std::cout << "[Renderer3D] Renderer initialized successfully." << std::endl;

	return true;
}

void Renderer3D::shutdown() {
	// Clean up resources
	if (vao_) {
		glDeleteVertexArrays(1, &vao_);
		vao_ = 0;
	}
	if (vbo_) {
		glDeleteBuffers(1, &vbo_);
		vbo_ = 0;
	}
	if (ebo_) {
		glDeleteBuffers(1, &ebo_);
		ebo_ = 0;
	}
	shaderLoaded_ = false;
	shader_ = 0;

	std::cout << "[Renderer3D] Renderer shutdown successfully." << std::endl;
}

void Renderer3D::setCurrentShape(CurrentShape shape) {
	currentShape_ = shape;
}
CurrentShape Renderer3D::getCurrentShape() const {
	return currentShape_;
}

void Renderer3D::beginScene(Shader& shader, const glm::mat4& view, const glm::mat4& proj) {
	glEnable(GL_DEPTH_TEST);
	// Culling
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	// glFrontFace(GL_CCW);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers

	// Use the shader program
	shader.use();

	// Save view and proj matrices
	view_ = view;
	proj_ = proj;
	// Set the model matrix to identity for now
	model_ = IDENTITY_MATRIX;

	// Reactivate the VAO for drawing
	glBindVertexArray(vao_);
}

void Renderer3D::drawTriangle(Shader& shader, const glm::mat4& transform) {
	// Set the model matrix
	model_ = transform;

	// Only upload geometry if we switched shapes
	if (currentShape_ != CurrentShape::TRIANGLE) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, triangleVertices_.size() * sizeof(Vertex3D), triangleVertices_.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices_.size() * sizeof(unsigned int), triangleIndices_.data(), GL_DYNAMIC_DRAW);

		currentShape_ = CurrentShape::TRIANGLE;
	}
	// Pass the matrices to the shader
	shader.setMat4("model", model_);
	shader.setMat4("view", view_);
	shader.setMat4("proj", proj_);

	// No need to set colour as a uniform. Colour info (per vertex) is already packed into the VBO.
	// Draw the triangle
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void Renderer3D::drawPlane(Shader& shader, const glm::mat4& transform) {
	// Set the model matrix
	model_ = transform;

	// Only upload geometry if we switched shapes
	if (currentShape_ != CurrentShape::PLANE) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, planeVertices_.size() * sizeof(Vertex3D), planeVertices_.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndices_.size() * sizeof(unsigned int), planeIndices_.data(), GL_DYNAMIC_DRAW);

		currentShape_ = CurrentShape::PLANE;
	}

	// Pass the matrices to the shader
	shader.setMat4("model", model_);
	shader.setMat4("view", view_);
	shader.setMat4("proj", proj_);

	// Draw the plane
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}