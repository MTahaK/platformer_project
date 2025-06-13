#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>


class Shader{

    public:
        Shader() = default;
        ~Shader();

        bool load(const std::string& vertexPath, const std::string& fragmentPath);
        void use() const;
        GLuint getID() const { return shaderID; }

        // Utility functions to set uniforms
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec2(const std::string& name, const glm::vec2& value) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setMat4(const std::string& name, const glm::mat4& mat) const;

    private:
        GLuint shaderID; // Shader program ID, 0 if load failed/at initialization
        std::string vertexPath_;
        std::string fragmentPath_;
        
        std::string loadShaderSource(const std::string& path);
        GLuint compileShader(const std::string& source, GLenum type);

};

// Renderer2D class for 2D rendering operations
class Renderer2D {

    public:
        Renderer2D() = default;
        ~Renderer2D();
        
        bool init(); // returns true on success, false on failure
        void shutdown();
        void beginScene();
        void drawQuad(const glm::mat4& transform, const glm::vec4& color);
        void endScene();

    private:

        GLuint shader_ = 0;
        GLuint vao_;
        GLuint vbo_;
        GLuint ebo_;

        bool shaderLoaded_ = false;
};

