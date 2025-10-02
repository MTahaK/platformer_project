#include <iostream>
#include <fstream>
#include <sstream>
#include "renderer2d.hpp"
#include "shader.hpp"
#include "texture.hpp"

Renderer2D::~Renderer2D() { shutdown(); }

bool Renderer2D::init(Shader& shader) {
	// Shader object is passed in, use directly
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

	// Define the vertex data for a quad
	float vertices[] = {
		// x, y, u, v
		-0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
		0.5f,  -0.5f, 1.0f, 0.0f, // Bottom-right
		0.5f,  0.5f,  1.0f, 1.0f, // Top-right
		-0.5f, 0.5f,  0.0f, 1.0f  // Top-left
	};

	// Indices for the quad
	unsigned int indices[] = {
		0, 1, 2, // First triangle
		2, 3, 0	 // Second triangle
	};

	// Upload vertex data to the VBO, create EBO, bind, and upload index data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set vertex attribute pointers

	// Vertex positions
	glVertexAttribPointer(0,				 // attribute location (in vertex shader)
						  2,				 // number of components (x, y)
						  GL_FLOAT,			 // data type
						  GL_FALSE,			 // normalize?
						  4*sizeof(float), // stride (bytes between vertices)
						  (void*)0			 // offset into data
	);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float))); // Texture
	glEnableVertexAttribArray(0); // Enable the vertex attribute at location 0
	glEnableVertexAttribArray(1); // Enable the vertex attribute at location 1

	// Unbind VAO to avoid accidental modification
	glBindVertexArray(0);

	// --- Player-specific dynamic quad setup ---
	glGenVertexArrays(1, &playerVAO_);
	glBindVertexArray(playerVAO_);

	glGenBuffers(1, &playerVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, playerVBO_);

	// Initialize with the same quad vertex layout; UVs will be updated per-frame
	float pVertices[] = {
		// x, y, u, v
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(pVertices), pVertices, GL_DYNAMIC_DRAW);

	// Bind the same EBO so index order matches
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

	// Attribute layout identical to generic VAO
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// Set up glClearColor
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Successful initialization
	std::cout << "[Renderer2D] Renderer initialized successfully." << std::endl;
	return true;
}

bool Renderer2D::initLine(Shader& shader) {
	// Shader object is passed in, use directly
	if (!shader.getID()) {
		std::cerr << "[Renderer2D] Shader program ID is 0, shader not loaded." << std::endl;
		return false; // Shader not loaded
	}
	shaderLoaded_ = true;
	shader_ = shader.getID();

	float vertices[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // Placeholder; will update each frame.

	// Set up vertex array object (VAO), vertex buffer object (VBO) for line
	glGenVertexArrays(1, &lineVAO_);
	glBindVertexArray(lineVAO_);

	glGenBuffers(1, &lineVBO_);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0,				 // attribute location (in vertex shader)
						  2,				 // number of components (x, y)
						  GL_FLOAT,			 // data type
						  GL_FALSE,			 // normalize?
						  2 * sizeof(float), // stride (bytes between vertices)
						  (void*)0			 // offset into data
	);

	glEnableVertexAttribArray(0); // Enable the vertex attribute at location 0

	// Unbind VAO to avoid accidental modification
	glBindVertexArray(0);

	return true;
}

void Renderer2D::shutdown() {
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

	std::cout << "[Renderer2D] Renderer shutdown successfully." << std::endl;
}

void Renderer2D::beginScene(Shader& shader, const glm::mat4& view, const glm::mat4& proj) {
	glClear(GL_COLOR_BUFFER_BIT); // Clear color

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

void Renderer2D::drawQuad(Shader& shader, const glm::mat4& transform, const glm::vec4& color) {
	// Ensure quad VAO is bound (other renderers may have changed it)
	glBindVertexArray(vao_);
	model_ = transform;

	glm::mat4 mvp = proj_ * view_ * model_;

	shader.setMat4("MVP", mvp);
	shader.setVec4("color", color); // Set the color uniform

	shader.setInt("useTexture", 0);
	shader.setInt("slot", 0);

	// Draw the quad using the EBO, VAO already bound in beginScene
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer2D::drawTexturedQuad(Shader& shader, const glm::mat4& transform, const glm::vec4& color, Texture *texture) {
	// Ensure quad VAO is bound
	glBindVertexArray(vao_);

	model_ = transform;

	glm::mat4 mvp = proj_ * view_ * model_;

	// Bind and configure texture
	texture->bind(0); // Bind to texture unit 0

	shader.setMat4("MVP", mvp);
	shader.setVec4("color", color);
	shader.setInt("useTexture", 1);
	shader.setInt("slot", 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer2D::setPlayerUVRect(const glm::vec2& uvMin, const glm::vec2& uvMax) {
	// Update only the UV components in the dynamic VBO
	// Layout per vertex: [x, y, u, v]
	struct UVPair { float u, v; };
	UVPair uvs[4];
	uvs[0] = { uvMin.x, uvMin.y };
	uvs[1] = { uvMax.x, uvMin.y };
	uvs[2] = { uvMax.x, uvMax.y };
	uvs[3] = { uvMin.x, uvMax.y };

	glBindBuffer(GL_ARRAY_BUFFER, playerVBO_);
	float playerVertices[16] = {
		-0.5f, -0.5f, uvs[0].u, uvs[0].v,
		 0.5f, -0.5f, uvs[1].u, uvs[1].v,
		 0.5f,  0.5f, uvs[2].u, uvs[2].v,
		-0.5f,  0.5f, uvs[3].u, uvs[3].v
	};
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(playerVertices), playerVertices);
}

void Renderer2D::drawPlayer(Shader& shader, const glm::mat4& transform, const glm::vec4& color, Texture* texture) {
	// Bind player VAO to use dynamic VBO UVs
	glBindVertexArray(playerVAO_);

	model_ = transform;
	glm::mat4 mvp = proj_ * view_ * model_;

	texture->bind(1);
	shader.setMat4("MVP", mvp);
	shader.setVec4("color", color);
	shader.setInt("useTexture", 1);
	shader.setInt("slot", 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer2D::drawLine(Shader& shader, const glm::vec2& start, const glm::vec2& end, const glm::vec4& color) {

	// Update the vertex buffer with the line endpoints
	float vertices[] = {start.x, start.y, end.x, end.y};
	glBindVertexArray(lineVAO_);

	glBindBuffer(GL_ARRAY_BUFFER, lineVBO_);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	shader.use();

	glm::mat4 mvp = proj_ * view_ * glm::mat4(1.0f);
	shader.setMat4("MVP", mvp);

	// Set the color uniform
	shader.setVec4("color", color);

	shader.setInt("useTexture", 0);
	shader.setInt("slot", 0);

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2.0f);

	glDrawArrays(GL_LINES, 0, 2); // Draw two vertices as a line

	// Restore previous VAO
	glBindVertexArray(vao_);
}
