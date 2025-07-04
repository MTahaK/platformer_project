#pragma once
#include <glm/glm.hpp>
#include <sstream>

glm::vec4 hexToRGBA(const std::string& hex) {
    unsigned int hexValue;
    std::stringstream ss;
    ss << std::hex << hex.substr(1); // Skip the '#' character
    ss >> hexValue;

    float r = ((hexValue >> 16) & 0xFF) / 255.0f;
    float g = ((hexValue >> 8) & 0xFF) / 255.0f;
    float b = (hexValue & 0xFF) / 255.0f;
    float a = 1.0f; // Default alpha value

    return glm::vec4(r, g, b, a);
}

glm::vec4 rgbaToVec4(const std::string& rgba) {
    unsigned int r, g, b, a;
    std::stringstream ss(rgba);
    char comma; // To skip commas
    ss >> r >> comma >> g >> comma >> b >> comma >> a;

    return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}