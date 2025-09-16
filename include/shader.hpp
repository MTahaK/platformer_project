#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>

class Shader {

	public:
		Shader();
		~Shader();

		// Load shaders, compile, and link into shader program
		bool load(const std::string& vertexPath, const std::string& fragmentPath);
		bool reload();
		void unload();
		void shutdown();

		void use() const;

		GLuint getID() const { return shaderID; }

		// Utility functions to set uniforms
		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setVec2(const std::string& name, const glm::vec2& value) const;
		void setVec3(const std::string& name, const glm::vec3& value) const;
		void setVec4(const std::string& name, const glm::vec4& value) const;
		void setMat3(const std::string& name, const glm::mat3& mat) const;
		void setMat4(const std::string& name, const glm::mat4& mat) const;

	private:
		GLuint shaderID; // Shader program ID, 0 if load failed/at initialization
		std::string vertexPath_;
		std::string fragmentPath_;

		mutable std::unordered_map<std::string, GLint> uniformLocCache_;

		std::string loadShaderSource(const std::string& path);
		GLuint compileShader(const std::string& source, GLenum type);
		GLint getUniformLocation(const std::string& name) const;
};
