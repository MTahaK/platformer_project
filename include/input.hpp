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

		static void clear();

		// --- Key State Queries ---
		static bool isKeyPressed(int key);		// Currently held down
		static bool isKeyJustPressed(int key);	// Transitioned from up to down this frame
		static bool isKeyJustReleased(int key); // Transitioned from down to up this frame

	private:
		static GLFWwindow* s_window;

		static std::vector<int> s_trackedKeys_;

		static std::unordered_map<int, int> s_currentKeys_;
		static std::unordered_map<int, int> s_previousKeys_;
};
