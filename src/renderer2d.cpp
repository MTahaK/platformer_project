#include "renderer2d.hpp"
#include "shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Renderer2D::~Renderer2D() {
    shutdown();
}

bool Renderer2D::init(Shader& shader){
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
        // x, y
        -0.5f, -0.5f, // Bottom-left
         0.5f, -0.5f, // Bottom-right
         0.5f,  0.5f, // Top-right
        -0.5f,  0.5f  // Top-left
    };
    
    // Indices for the quad
    unsigned int indices[] = {
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
    };

    // Upload vertex data to the VBO, create EBO, bind, and upload index data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
        glVertexAttribPointer(
        0,                  // attribute location (in vertex shader)
        2,                  // number of components (x, y)
        GL_FLOAT,           // data type
        GL_FALSE,           // normalize?
        2 * sizeof(float),  // stride (bytes between vertices)
        (void*)0            // offset into data
    );

    glEnableVertexAttribArray(0); // Enable the vertex attribute at location 0

    // Unbind VAO to avoid accidental modification
    glBindVertexArray(0);

    // Successful initialization
    std::cout << "[Renderer2D] Renderer initialized successfully." << std::endl;
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

void Renderer2D::beginScene(Shader& shader, const glm::mat4& view, const glm::mat4& proj){
    // Set up clear color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark blue
    glClear(GL_COLOR_BUFFER_BIT); // Clear color

    // Use the shader program
    shader.use();

    // Set the view and projection matrices in the shader
    // shader.setMat4("view", view);
    // shader.setMat4("projection", proj);

    // Save view and proj matrices
    view_ = view;
    proj_ = proj;
    // Set the model matrix to identity for now
    model_ = glm::mat4(1.0f);

    // Reactivate the VAO for drawing
    glBindVertexArray(vao_);
}

void Renderer2D::drawQuad(Shader& shader, const glm::mat4& transform, const glm::vec4& color) {
    // Set the model matrix to the provided transform
    model_ = transform;

    // Compute MVP
    glm::mat4 mvp = proj_ * view_ * model_;

    // Set the MVP matrix in the shader
    shader.setMat4("MVP", mvp);
    shader.setVec4("color", color); // Set the color uniform

    // Draw the quad using the EBO, VAO already bound in beginScene
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}