#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Texture {
	GLuint id;
	int width;
	int height;
	int channels;
};