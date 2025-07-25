#pragma once
#include <glm/glm.hpp>
#include <sstream>

glm::vec4 hexToVec4(const std::string& hex);
glm::vec4 rgbaToVec4(const std::string& rgba);