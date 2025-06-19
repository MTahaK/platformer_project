#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>

// Input class handles polling-style keyboard and mouse input
// For now, assumes a single window and static usage pattern
class Input {
public:
    // Initialize with the active GLFW window
    static void initialize(GLFWwindow* window);

    // Update key/mouse state (called once per frame)
    static void update();

    // --- Key State Queries ---
    static bool isKeyPressed(int key);      // Currently held down
    static bool isKeyJustPressed(int key);  // Transitioned from up to down this frame
    static bool isKeyJustReleased(int key); // Transitioned from down to up this frame


private:
    static GLFWwindow* s_window;

    static std::unordered_map<int, int> s_curr;
    static std::unordered_map<int, int> s_prev;;

};

// Vector of keys that are tracked for input state
std::vector<int> tracked_keys = {
    GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE,
    GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN,
    GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E,
    GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L,
    GLFW_KEY_Z, GLFW_KEY_X
};