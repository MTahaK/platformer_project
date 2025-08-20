#include "shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader() {
	// Initialize shaderID to 0, indicating no shader program is loaded
	shaderID = 0;
}
Shader::~Shader() { shutdown(); }

void Shader::shutdown() {
	// Deletes the shader program if it exists
	if (shaderID != 0) {
		glDeleteProgram(shaderID);
		shaderID = 0; // Reset to 0 after deletion
	}
}

std::string Shader::loadShaderSource(const std::string& path) {
	// Reads from shader file, creates buffer containing contents
	// of shader file
	std::ifstream file(path);
	std::stringstream buf;

	if (!file.is_open()) {
		std::cerr << "[Shader] Failed to open shader file: " << path << ". Load aborting...\n";
		return "";
	}

	buf << file.rdbuf(); // Read the entire file into the buffer
	file.close();
	return buf.str();
}

GLuint Shader::compileShader(const std::string& source, GLenum type) {

	// Compile a shader from source code
	GLuint shader = glCreateShader(type); // type is GL_VERTEX_SHADER or GL_FRAGMENT_SHADER

	const char* src = source.c_str(); /// Needs to be a C-style string
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	// Check for compilation errors
	int pass;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &pass);

	if (pass == GL_FALSE) {
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		std::string log(length, ' ');
		glGetShaderInfoLog(shader, length, &length, &log[0]);
		std::cerr << "[Shader] Compilation failed: " << log << std::endl;
		glDeleteShader(shader);
		return 0; // Return 0 on failure
	}
	return shader; // Return the shader ID on success
}

bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath) {

	// Load vertex and fragment shaders from files, compile them, and link them into a shader program

	// Before loading in, check if the shader has already been loaded
	if (shaderID != 0) {
		std::cerr << "[Shader] Shader already loaded. Cannot load again." << std::endl;
		return false;
	}

	vertexPath_ = vertexPath;
	fragmentPath_ = fragmentPath;

	std::string vertexSource = loadShaderSource(vertexPath_);
	if (vertexSource.empty()) {
		std::cerr << "[Shader] Failed to load vertex shader source from: " << vertexPath_ << std::endl;
		return false;
	}
	std::string fragmentSource = loadShaderSource(fragmentPath_);
	if (fragmentSource.empty()) {
		std::cerr << "[Shader] Failed to load fragment shader source from: " << fragmentPath_ << std::endl;
		return false;
	}

	// Shaders are loaded into memory buffers, now compile

	GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
	if (vertexShader == 0) {
		std::cerr << "[Shader] Vertex shader compilation failed." << std::endl;
		return false;
	}
	GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
	if (fragmentShader == 0) {
		std::cerr << "[Shader] Fragment shader compilation failed." << std::endl;
		glDeleteShader(vertexShader); // Clean up vertex shader, since already compiled here
		return false;
	}

	// Shaders compiled successfully, now link them into a shader program
	shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);

	glLinkProgram(shaderID);
	int pass;
	// Check for linking errors
	glGetProgramiv(shaderID, GL_LINK_STATUS, &pass);
	if (pass == GL_FALSE) {
		int length;
		glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		std::string log(length, ' ');
		glGetProgramInfoLog(shaderID, length, &length, &log[0]);
		std::cerr << "[Shader] Linking failed: " << log << std::endl;
		glDeleteProgram(shaderID);
		shaderID = 0; // Reset shaderID to indicate failure
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return false;
	}

	// Successfully linked, now delete the individual shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true; // Return true on success
}

void Shader::unload() {
    if (shaderID != 0) {
        glDeleteProgram(shaderID);
        shaderID = 0;
        uniformLocCache_.clear();  // Clear the uniform cache too
        std::cout << "[Shader] Shader unloaded." << std::endl;
    }
}

bool Shader::reload() {
    if (vertexPath_.empty() || fragmentPath_.empty()) {
        std::cerr << "[Shader] Cannot reload: no previous paths stored." << std::endl;
        return false;
    }
    
    std::cout << "[Shader] Reloading shader: " << vertexPath_ << ", " << fragmentPath_ << std::endl;
    
    // Unload current shader
    unload();
    
    // Load again using stored paths
    return load(vertexPath_, fragmentPath_);
}

void Shader::use() const {
	// Use the shader program
	if (shaderID != 0) {
		glUseProgram(shaderID);
	} else {
		std::cerr << "[Shader] Cannot use shader, shaderID is 0." << std::endl;
	}
}

GLint Shader::getUniformLocation(const std::string& name) const {
	auto it = uniformLocCache_.find(name);
	if (it != uniformLocCache_.end()) {
		return it->second;
	}

	GLint location = glGetUniformLocation(shaderID, name.c_str());
	if (location != -1) {
		uniformLocCache_[name] = location;
	}
	return location;
}

void Shader::setBool(const std::string& name, bool value) const { glUniform1i(getUniformLocation(name), (int)value); }
void Shader::setInt(const std::string& name, int value) const { glUniform1i(getUniformLocation(name), value); }
void Shader::setFloat(const std::string& name, float value) const { glUniform1f(getUniformLocation(name), value); }
void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}
