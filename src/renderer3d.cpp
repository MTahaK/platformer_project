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
	setShader(shader);

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

	// Cube vertices (8 vertices, each vertex has varied colors with green components)
	cubeVertices_ = {
		// Front face (red-green mix)
		{{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // 0: Bottom-left-front (red)
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // 1: Bottom-right-front (green)
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // 2: Top-right-front (yellow)
		{{-0.5f,  0.5f,  0.5f}, {0.5f, 1.0f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // 3: Top-left-front (light green)
		
		// Back face (blue-green mix)
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}}, // 4: Bottom-left-back (blue)
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}}, // 5: Bottom-right-back (cyan)
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}}, // 6: Top-right-back (magenta)
		{{-0.5f,  0.5f, -0.5f}, {0.5f, 0.5f, 1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}}  // 7: Top-left-back (light blue)
	};

	cubeIndices_ = {
		// Front face
		0, 1, 2,  2, 3, 0,
		// Back face
		4, 6, 5,  6, 4, 7,
		// Left face
		4, 0, 3,  3, 7, 4,
		// Right face
		1, 5, 6,  6, 2, 1,
		// Top face
		3, 2, 6,  6, 7, 3,
		// Bottom face
		4, 5, 1,  1, 0, 4
	};

	// Pyramid vertices (5 vertices: 4 base + 1 apex)
	pyramidVertices_ = {
		// Base vertices (square base in XZ plane)
		{{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}, // 0: Bottom-left
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}, // 1: Bottom-right
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}, // 2: Top-right
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}, // 3: Top-left
		
		// Apex vertex
		{{ 0.0f,  0.5f,  0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}   // 4: Apex (magenta)
	};

	pyramidIndices_ = {
		// Base (two triangles)
		0, 2, 1,  0, 3, 2,
		// Side faces (4 triangles pointing to apex)
		0, 1, 4,  // Front face
		1, 2, 4,  // Right face
		2, 3, 4,  // Back face
		3, 0, 4   // Left face
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

void Renderer3D::beginScene(const glm::mat4& view, const glm::mat4& proj) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the current shader
	if (currentShader_) {
		currentShader_->use();
	}

	// Save matrices
	view_ = view;
	proj_ = proj;
	model_ = IDENTITY_MATRIX;

	glBindVertexArray(vao_);
}

void Renderer3D::setShader(Shader& shader) {
	currentShader_ = &shader;
	shader_ = shader.getID();
}

void Renderer3D::setLightingUniforms(const glm::vec3& lightPos, const glm::vec3& viewPos, const glm::vec3& lightColor) {
	if (currentShader_) {
		currentShader_->setVec3("lightPos", lightPos);
		currentShader_->setVec3("viewPos", viewPos);
		currentShader_->setVec3("lightColor", lightColor);
	}
}

void Renderer3D::drawTriangle(const glm::mat4& transform) {
	model_ = transform;

	if (currentShape_ != CurrentShape::TRIANGLE) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, triangleVertices_.size() * sizeof(Vertex3D), triangleVertices_.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices_.size() * sizeof(unsigned int), triangleIndices_.data(), GL_DYNAMIC_DRAW);

		currentShape_ = CurrentShape::TRIANGLE;
	}
	
	if (currentShader_) {
		currentShader_->setMat4("model", model_);
		currentShader_->setMat4("view", view_);
		currentShader_->setMat4("proj", proj_);
	}

	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void Renderer3D::drawPlane(const glm::mat4& transform) {
	model_ = transform;

	if (currentShape_ != CurrentShape::PLANE) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, planeVertices_.size() * sizeof(Vertex3D), planeVertices_.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndices_.size() * sizeof(unsigned int), planeIndices_.data(), GL_DYNAMIC_DRAW);

		currentShape_ = CurrentShape::PLANE;
	}

	if (currentShader_) {
		currentShader_->setMat4("model", model_);
		currentShader_->setMat4("view", view_);
		currentShader_->setMat4("proj", proj_);
	}

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer3D::drawCube(const glm::mat4& transform) {
	model_ = transform;

	if (currentShape_ != CurrentShape::CUBE) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, cubeVertices_.size() * sizeof(Vertex3D), cubeVertices_.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices_.size() * sizeof(unsigned int), cubeIndices_.data(), GL_DYNAMIC_DRAW);

		currentShape_ = CurrentShape::CUBE;
	}

	if (currentShader_) {
		currentShader_->setMat4("model", model_);
		currentShader_->setMat4("view", view_);
		currentShader_->setMat4("proj", proj_);
	}

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer3D::drawPyramid(const glm::mat4& transform) {
	model_ = transform;

	if (currentShape_ != CurrentShape::PYRAMID) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, pyramidVertices_.size() * sizeof(Vertex3D), pyramidVertices_.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pyramidIndices_.size() * sizeof(unsigned int), pyramidIndices_.data(), GL_DYNAMIC_DRAW);

		currentShape_ = CurrentShape::PYRAMID;
	}

	if (currentShader_) {
		currentShader_->setMat4("model", model_);
		currentShader_->setMat4("view", view_);
		currentShader_->setMat4("proj", proj_);
	}

	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
}