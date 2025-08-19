#include "input.hpp"

// Define static members
GLFWwindow* Input::s_window = nullptr;

std::unordered_map<int, int> Input::s_currentKeys_;
std::unordered_map<int, int> Input::s_previousKeys_;

void Input::initialize(GLFWwindow* window) {
	s_window = window;
	// Might try pre-filling key map to improve performance later
}

void Input::update() {
	s_previousKeys_ = s_currentKeys_; // Store previous state

	// Iterate only over tracked keys
	// Update logic for tracked keys may come if remapping added
	for (int key : s_trackedKeys_) {
		int state = glfwGetKey(s_window, key);
		// 0 -> GLFW_RELEASED, 1-> GLFW_PRESS
		s_currentKeys_[key] = state;
	}
}

void Input::clear() {
	// Clear input state by updating twice - this moves current to previous
	// and ensures no "just pressed" states carry over
	update();
	update();
}

bool Input::isKeyPressed(int key) {
	auto it = s_currentKeys_.find(key);
	return it != s_currentKeys_.end() && it->second == GLFW_PRESS;
	// Prevents insertion of unadded keys into map
}

bool Input::isKeyJustPressed(int key) {
	auto itCurrent = s_currentKeys_.find(key);
	auto itPrevious = s_previousKeys_.find(key);
	// Calling find() on the key prevents insertion
	if (itCurrent != s_currentKeys_.end() && itPrevious != s_previousKeys_.end()) {
		if (itCurrent->second == GLFW_PRESS && itPrevious->second == GLFW_RELEASE) {
			return true;
		}
	}
	return false;
}

bool Input::isKeyJustReleased(int key) {
	auto itCurrent = s_currentKeys_.find(key);
	auto itPrevious = s_previousKeys_.find(key);
	// Calling find() on the key prevents insertion
	if (itCurrent != s_currentKeys_.end() && itPrevious != s_previousKeys_.end()) {
		if (itCurrent->second == GLFW_RELEASE && itPrevious->second == GLFW_PRESS) {
			return true;
		}
	}
	return false;
}

std::vector<int> Input::s_trackedKeys_ = {
	GLFW_KEY_SPACE,		 GLFW_KEY_ENTER, GLFW_KEY_ESCAPE, GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN,
	GLFW_KEY_LEFT_SHIFT, GLFW_KEY_W,	 GLFW_KEY_A,	  GLFW_KEY_S,	 GLFW_KEY_D,	 GLFW_KEY_Q,  GLFW_KEY_E,
	GLFW_KEY_F,			 GLFW_KEY_G,	 GLFW_KEY_H,	  GLFW_KEY_J,	 GLFW_KEY_K,	 GLFW_KEY_L,  GLFW_KEY_Z,
	GLFW_KEY_X,			 GLFW_KEY_Y,	 GLFW_KEY_T,	  GLFW_KEY_P,	 GLFW_KEY_O,	 GLFW_KEY_1,  GLFW_KEY_2,
	GLFW_KEY_3,			 GLFW_KEY_4,	 GLFW_KEY_F1,	  GLFW_KEY_F2,	 GLFW_KEY_F3,	 GLFW_KEY_F4,
};