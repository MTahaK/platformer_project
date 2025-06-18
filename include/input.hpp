#pragma once
#include <GLFW/glfw3.h>
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
    static GLFWwindow* s_Window;

    static std::unordered_map<int, bool> s_CurrentKeys;
    static std::unordered_map<int, bool> s_PreviousKeys;

};
