#include "renderer3d.hpp"
#include "shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
		{{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},	  // Top right, red
		{{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  // Top left, green
		{{-0.5f, -0.5f, 0.0f}, {0.5f, 0.0f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}}, // Bottom left, purple
		{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  // Bottom right, blue
	};

	planeIndices_ = {0, 1, 2, 2, 3, 0};

	cubeVertices_ = {
		// +Z (front)
		{{-0.5f,-0.5f, 0.5f}, {1.0f,0.0f,0.0f,1.0f}, { 0.0f, 0.0f, 1.0f}}, // BLF red
		{{ 0.5f,-0.5f, 0.5f}, {0.0f,1.0f,0.0f,1.0f}, { 0.0f, 0.0f, 1.0f}}, // BRF green
		{{ 0.5f, 0.5f, 0.5f}, {1.0f,1.0f,0.0f,1.0f}, { 0.0f, 0.0f, 1.0f}}, // TRF yellow
		{{-0.5f, 0.5f, 0.5f}, {0.5f,1.0f,0.5f,1.0f}, { 0.0f, 0.0f, 1.0f}}, // TLF light green

		// -Z (back)
		{{ 0.5f,-0.5f,-0.5f}, {0.0f,1.0f,1.0f,1.0f}, { 0.0f, 0.0f,-1.0f}}, // BRB cyan
		{{-0.5f,-0.5f,-0.5f}, {0.0f,0.0f,1.0f,1.0f}, { 0.0f, 0.0f,-1.0f}}, // BLB blue
		{{-0.5f, 0.5f,-0.5f}, {0.5f,0.5f,1.0f,1.0f}, { 0.0f, 0.0f,-1.0f}}, // TLB light blue
		{{ 0.5f, 0.5f,-0.5f}, {1.0f,0.0f,1.0f,1.0f}, { 0.0f, 0.0f,-1.0f}}, // TRB magenta

		// -X (left)
		{{-0.5f,-0.5f,-0.5f}, {0.0f,0.0f,1.0f,1.0f}, {-1.0f, 0.0f, 0.0f}}, // blue
		{{-0.5f,-0.5f, 0.5f}, {1.0f,0.0f,0.0f,1.0f}, {-1.0f, 0.0f, 0.0f}}, // red
		{{-0.5f, 0.5f, 0.5f}, {0.5f,1.0f,0.5f,1.0f}, {-1.0f, 0.0f, 0.0f}}, // light green
		{{-0.5f, 0.5f,-0.5f}, {0.5f,0.5f,1.0f,1.0f}, {-1.0f, 0.0f, 0.0f}}, // light blue

		// +X (right)
		{{ 0.5f,-0.5f, 0.5f}, {0.0f,1.0f,0.0f,1.0f}, { 1.0f, 0.0f, 0.0f}}, // green
		{{ 0.5f,-0.5f,-0.5f}, {0.0f,1.0f,1.0f,1.0f}, { 1.0f, 0.0f, 0.0f}}, // cyan
		{{ 0.5f, 0.5f,-0.5f}, {1.0f,0.0f,1.0f,1.0f}, { 1.0f, 0.0f, 0.0f}}, // magenta
		{{ 0.5f, 0.5f, 0.5f}, {1.0f,1.0f,0.0f,1.0f}, { 1.0f, 0.0f, 0.0f}}, // yellow

		// +Y (top)
		{{-0.5f, 0.5f, 0.5f}, {0.5f,1.0f,0.5f,1.0f}, { 0.0f, 1.0f, 0.0f}}, // light green
		{{ 0.5f, 0.5f, 0.5f}, {1.0f,1.0f,0.0f,1.0f}, { 0.0f, 1.0f, 0.0f}}, // yellow
		{{ 0.5f, 0.5f,-0.5f}, {1.0f,0.0f,1.0f,1.0f}, { 0.0f, 1.0f, 0.0f}}, // magenta
		{{-0.5f, 0.5f,-0.5f}, {0.5f,0.5f,1.0f,1.0f}, { 0.0f, 1.0f, 0.0f}}, // light blue

		// -Y (bottom)
		{{-0.5f,-0.5f,-0.5f}, {0.0f,0.0f,1.0f,1.0f}, { 0.0f,-1.0f, 0.0f}}, // blue
		{{ 0.5f,-0.5f,-0.5f}, {0.0f,1.0f,1.0f,1.0f}, { 0.0f,-1.0f, 0.0f}}, // cyan
		{{ 0.5f,-0.5f, 0.5f}, {0.0f,1.0f,0.0f,1.0f}, { 0.0f,-1.0f, 0.0f}}, // green
		{{-0.5f,-0.5f, 0.5f}, {1.0f,0.0f,0.0f,1.0f}, { 0.0f,-1.0f, 0.0f}}, // red
	};

	cubeIndices_ = {
		// +Z
		0, 1, 2, 0, 2, 3,
		// -Z
		4, 5, 6, 4, 6, 7,
		// -X
		8, 9, 10, 8, 10, 11,
		// +X
		12, 13, 14, 12, 14, 15,
		// +Y
		16, 17, 18, 16, 18, 19,
		// -Y
		20, 21, 22, 20, 22, 23
	};

	lightCubeVertices_ = {
		// +Z (front)
		{{-0.5f,-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f}}, // BLF red
		{{ 0.5f,-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f}}, // BRF green
		{{ 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f}}, // TRF yellow
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f}}, // TLF light green

		// -Z (back)
		{{ 0.5f,-0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f,-1.0f}}, // BRB cyan
		{{-0.5f,-0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f,-1.0f}}, // BLB blue
		{{-0.5f, 0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f,-1.0f}}, // TLB light blue
		{{ 0.5f, 0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f,-1.0f}}, // TRB magenta

		// -X (left)
		{{-0.5f,-0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}}, // blue
		{{-0.5f,-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}}, // red
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}}, // light green
		{{-0.5f, 0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}}, // light blue

		// +X (right)
		{{ 0.5f,-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}}, // green
		{{ 0.5f,-0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}}, // cyan
		{{ 0.5f, 0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}}, // magenta
		{{ 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}}, // yellow

		// +Y (top)
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f, 0.0f}}, // light green
		{{ 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f, 0.0f}}, // yellow
		{{ 0.5f, 0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f, 0.0f}}, // magenta
		{{-0.5f, 0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f, 0.0f}}, // light blue

		// -Y (bottom)
		{{-0.5f,-0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f,-1.0f, 0.0f}}, // blue
		{{ 0.5f,-0.5f,-0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f,-1.0f, 0.0f}}, // cyan
		{{ 0.5f,-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f,-1.0f, 0.0f}}, // green
		{{-0.5f,-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f,-1.0f, 0.0f}}, // red
	};

	lightCubeIndices_ = cubeIndices_;
	// Side normals (normalized): 1/sqrt(5)=0.4472136, 2/sqrt(5)=0.8944272
	const float NY = 0.4472136f;
	const float NZ = 0.8944272f;
	const float NX = 0.8944272f;

	pyramidVertices_ = {
		// Base (down-facing)
		{{-0.5f,-0.5f, 0.5f}, {1.0f,0.0f,0.0f,1.0f}, { 0.0f,-1.0f, 0.0f}}, // 0 red
		{{ 0.5f,-0.5f, 0.5f}, {0.0f,1.0f,0.0f,1.0f}, { 0.0f,-1.0f, 0.0f}}, // 1 green
		{{ 0.5f,-0.5f,-0.5f}, {0.0f,0.0f,1.0f,1.0f}, { 0.0f,-1.0f, 0.0f}}, // 2 blue
		{{-0.5f,-0.5f,-0.5f}, {1.0f,1.0f,0.0f,1.0f}, { 0.0f,-1.0f, 0.0f}}, // 3 yellow

		// Front side (z = +0.5)
		{{-0.5f,-0.5f, 0.5f}, {1.0f,0.0f,0.0f,1.0f}, { 0.0f, NY,  NZ}}, // red
		{{ 0.5f,-0.5f, 0.5f}, {0.0f,1.0f,0.0f,1.0f}, { 0.0f, NY,  NZ}}, // green
		{{ 0.0f, 0.5f, 0.0f}, {1.0f,0.0f,1.0f,1.0f}, { 0.0f, NY,  NZ}}, // magenta apex

		// Right side (x = +0.5)
		{{ 0.5f,-0.5f, 0.5f}, {0.0f,1.0f,0.0f,1.0f}, { NX,  NY,  0.0f}}, // green
		{{ 0.5f,-0.5f,-0.5f}, {0.0f,0.0f,1.0f,1.0f}, { NX,  NY,  0.0f}}, // blue
		{{ 0.0f, 0.5f, 0.0f}, {1.0f,0.0f,1.0f,1.0f}, { NX,  NY,  0.0f}}, // magenta apex

		// Back side (z = -0.5)
		{{ 0.5f,-0.5f,-0.5f}, {0.0f,0.0f,1.0f,1.0f}, { 0.0f, NY, -NZ}}, // blue
		{{-0.5f,-0.5f,-0.5f}, {1.0f,1.0f,0.0f,1.0f}, { 0.0f, NY, -NZ}}, // yellow
		{{ 0.0f, 0.5f, 0.0f}, {1.0f,0.0f,1.0f,1.0f}, { 0.0f, NY, -NZ}}, // magenta apex

		// Left side (x = -0.5)
		{{-0.5f,-0.5f,-0.5f}, {1.0f,1.0f,0.0f,1.0f}, {-NX,  NY,  0.0f}}, // yellow
		{{-0.5f,-0.5f, 0.5f}, {1.0f,0.0f,0.0f,1.0f}, {-NX,  NY,  0.0f}}, // red
		{{ 0.0f, 0.5f, 0.0f}, {1.0f,0.0f,1.0f,1.0f}, {-NX,  NY,  0.0f}}, // magenta apex
	};

	pyramidIndices_ = {
		// Base (CCW when viewed from below; outward normal -Y)
		0,2,1,  0,3,2,

		// Sides
		4,5,6,      // front
		7,8,9,      // right
		10,11,12,   // back
		13,14,15    // left
	};

	if (sphereVertices_.empty()) {
		const int segments = 32; // Horizontal segments
		const int rings = 16;	 // Vertical rings
		const float radius = 1.0f;

		sphereVertices_.clear();
		sphereIndices_.clear();

		// Generate vertices
		for (int ring = 0; ring <= rings; ring++) {
			float phi = M_PI * float(ring) / float(rings); // Vertical angle
			float y = cos(phi) * radius;
			float ringRadius = sin(phi) * radius;

			for (int seg = 0; seg <= segments; seg++) {
				float theta = 2.0f * M_PI * float(seg) / float(segments); // Horizontal angle
				float x = ringRadius * cos(theta);
				float z = ringRadius * sin(theta);

				// Position
				glm::vec3 pos(x, y, z);

				// Normal (for a unit sphere, normal equals position)
				glm::vec3 normal = normalize(pos);

				glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

				sphereVertices_.push_back({pos, color, normal});
			}
		}

		// Generate indices for triangles
		for (int ring = 0; ring < rings; ring++) {
			for (int seg = 0; seg < segments; seg++) {
				int current = ring * (segments + 1) + seg;
				int next = current + segments + 1;

				// Two triangles per quad
				sphereIndices_.push_back(current);
				sphereIndices_.push_back(current + 1);
				sphereIndices_.push_back(next);

				sphereIndices_.push_back(current + 1);
				sphereIndices_.push_back(next + 1);
				sphereIndices_.push_back(next);
			}
		}
	}

	glGenBuffers(1, &triangleVBO_);
	glGenBuffers(1, &triangleEBO_);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO_);
	glBufferData(GL_ARRAY_BUFFER, triangleVertices_.size() * sizeof(Vertex3D), triangleVertices_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices_.size() * sizeof(unsigned int), triangleIndices_.data(), GL_DYNAMIC_DRAW);

	glGenBuffers(1, &planeVBO_);
	glGenBuffers(1, &planeEBO_);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO_);
	glBufferData(GL_ARRAY_BUFFER, planeVertices_.size() * sizeof(Vertex3D), planeVertices_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndices_.size() * sizeof(unsigned int), planeIndices_.data(), GL_DYNAMIC_DRAW);


	glGenBuffers(1, &cubeVBO_);
	glGenBuffers(1, &cubeEBO_);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_);
	glBufferData(GL_ARRAY_BUFFER, cubeVertices_.size() * sizeof(Vertex3D), cubeVertices_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices_.size() * sizeof(unsigned int), cubeIndices_.data(), GL_DYNAMIC_DRAW);

	glGenBuffers(1, &lightCubeVBO_);
	glGenBuffers(1, &lightCubeEBO_);
	glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO_);
	glBufferData(GL_ARRAY_BUFFER, lightCubeVertices_.size() * sizeof(Vertex3D), lightCubeVertices_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightCubeEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lightCubeIndices_.size() * sizeof(unsigned int), lightCubeIndices_.data(), GL_DYNAMIC_DRAW);

	glGenBuffers(1, &pyramidVBO_);
	glGenBuffers(1, &pyramidEBO_);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO_);
	glBufferData(GL_ARRAY_BUFFER, pyramidVertices_.size() * sizeof(Vertex3D), pyramidVertices_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pyramidIndices_.size() * sizeof(unsigned int), pyramidIndices_.data(), GL_DYNAMIC_DRAW);

	glGenBuffers(1, &sphereVBO_);
	glGenBuffers(1, &sphereEBO_);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO_);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices_.size() * sizeof(Vertex3D), sphereVertices_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices_.size() * sizeof(unsigned int), sphereIndices_.data(), GL_DYNAMIC_DRAW);

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
    if (triangleVBO_) {
        glDeleteBuffers(1, &triangleVBO_);
        triangleVBO_ = 0;
    }
    if (triangleEBO_) {
        glDeleteBuffers(1, &triangleEBO_);
        triangleEBO_ = 0;
    }
    if (planeVBO_) {
        glDeleteBuffers(1, &planeVBO_);
        planeVBO_ = 0;
    }
    if (planeEBO_) {
        glDeleteBuffers(1, &planeEBO_);
        planeEBO_ = 0;
    }
    if (cubeVBO_) {
        glDeleteBuffers(1, &cubeVBO_);
        cubeVBO_ = 0;
    }
    if (cubeEBO_) {
        glDeleteBuffers(1, &cubeEBO_);
        cubeEBO_ = 0;
    }
    if (lightCubeVBO_) {
        glDeleteBuffers(1, &lightCubeVBO_);
        lightCubeVBO_ = 0;
    }
    if (lightCubeEBO_) {
        glDeleteBuffers(1, &lightCubeEBO_);
        lightCubeEBO_ = 0;
    }
    if (pyramidVBO_) {
        glDeleteBuffers(1, &pyramidVBO_);
        pyramidVBO_ = 0;
    }
    if (pyramidEBO_) {
        glDeleteBuffers(1, &pyramidEBO_);
        pyramidEBO_ = 0;
    }
    if (sphereVBO_) {
        glDeleteBuffers(1, &sphereVBO_);
        sphereVBO_ = 0;
    }
    if (sphereEBO_) {
        glDeleteBuffers(1, &sphereEBO_);
        sphereEBO_ = 0;
    }
    shaderLoaded_ = false;
    shader_ = 0;

    std::cout << "[Renderer3D] Renderer shutdown successfully." << std::endl;
}

void Renderer3D::setCurrentShape(CurrentShape shape) { currentShape_ = shape; }
CurrentShape Renderer3D::getCurrentShape() const { return currentShape_; }

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

	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO_);
	glBufferData(GL_ARRAY_BUFFER, triangleVertices_.size() * sizeof(Vertex3D), triangleVertices_.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices_.size() * sizeof(unsigned int), triangleIndices_.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
	glEnableVertexAttribArray(2);

	currentShape_ = CurrentShape::TRIANGLE;

	if (currentShader_) {
		currentShader_->setMat4("model", model_);
		currentShader_->setMat4("view", view_);
		currentShader_->setMat4("proj", proj_);
	}

	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void Renderer3D::drawPlane(const glm::mat4& transform) {
	model_ = transform;

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO_);
	glBufferData(GL_ARRAY_BUFFER, planeVertices_.size() * sizeof(Vertex3D), planeVertices_.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndices_.size() * sizeof(unsigned int), planeIndices_.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
	glEnableVertexAttribArray(2);
	currentShape_ = CurrentShape::PLANE;

	if (currentShader_) {
		currentShader_->setMat4("model", model_);
		currentShader_->setMat4("view", view_);
		currentShader_->setMat4("proj", proj_);
	}

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer3D::drawCube(const glm::mat4& transform) {
	model_ = transform;

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO_);
	glBufferData(GL_ARRAY_BUFFER, cubeVertices_.size() * sizeof(Vertex3D), cubeVertices_.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices_.size() * sizeof(unsigned int), cubeIndices_.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
	glEnableVertexAttribArray(2);

	currentShape_ = CurrentShape::CUBE;

	if (currentShader_) {
		currentShader_->setMat4("model", model_);
		currentShader_->setMat4("view", view_);
		currentShader_->setMat4("proj", proj_);
	}

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer3D::drawLightCube(const glm::mat4& transform, const glm::vec4& color) {
	model_ = transform;

	glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO_);
	glBufferData(GL_ARRAY_BUFFER, lightCubeVertices_.size() * sizeof(Vertex3D), lightCubeVertices_.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightCubeEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, lightCubeIndices_.size() * sizeof(unsigned int), lightCubeIndices_.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
	glEnableVertexAttribArray(2);

	currentShape_ = CurrentShape::CUBE;

	if (currentShader_) {
		currentShader_->setMat4("model", model_);
		currentShader_->setMat4("view", view_);
		currentShader_->setMat4("proj", proj_);
		currentShader_->setVec4("color", color);
	}

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer3D::drawPyramid(const glm::mat4& transform) {
	model_ = transform;

	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO_);
	glBufferData(GL_ARRAY_BUFFER, pyramidVertices_.size() * sizeof(Vertex3D), pyramidVertices_.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pyramidIndices_.size() * sizeof(unsigned int), pyramidIndices_.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
	glEnableVertexAttribArray(2);

	currentShape_ = CurrentShape::PYRAMID;

	if (currentShader_) {
		currentShader_->setMat4("model", model_);
		currentShader_->setMat4("view", view_);
		currentShader_->setMat4("proj", proj_);
	}

	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
}


void Renderer3D::drawSphere(const glm::mat4& transform) {
	model_ = transform;

	// Upload to GPU
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO_);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices_.size() * sizeof(Vertex3D), sphereVertices_.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices_.size() * sizeof(unsigned int), sphereIndices_.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
	glEnableVertexAttribArray(2);

	currentShape_ = CurrentShape::SPHERE;


	if (currentShader_) {
		currentShader_->setMat4("model", model_);
		currentShader_->setMat4("view", view_);
		currentShader_->setMat4("proj", proj_);
	}

	glDrawElements(GL_TRIANGLES, sphereIndices_.size(), GL_UNSIGNED_INT, 0);
}