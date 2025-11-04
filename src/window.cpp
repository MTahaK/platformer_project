#include "window.hpp"
#include <iostream>

bool Window::glfwInitialized_ = false;

bool Window::createWindow(int width, int height, const std::string& title) {

	// First, ensure that a window has not already been initialized
	if (glfwInitialized_) {
		std::cerr << "[Window] GLFW has already been initialized." << std::endl;
		return false;
	}
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "[Window] Failed to initialize GLFW." << std::endl;
		return false;
	}
	glfwInitialized_ = true;

	// Set window hints (for OpenGL 3.3 Core)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);				   // Specifies OpenGL 3.x
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);				   // Specifies OpenGL x.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL Core

	// Create the GLFW window
	window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window_) {
		std::cerr << "[Window] Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return false;
	}

	// Set OpenGL context to newly created (ie current) window
	glfwMakeContextCurrent(window_);
	glfwSwapInterval(1); // Enables vsync (1 = on, 0 = off)

	// Load OpenGL function pointers using GLAD
	if (!gladLoadGL()) {
		std::cerr << "[Window] Failed to initialize GLAD." << std::endl;
		// glfwDestroyWindow(window_);
		// glfwTerminate();
		destroy();
		return false;
	}

	// The GLFW window viewport must resize to match the window (ie framebuffer) size
	// Therefore, first get the framebuffer size and set the framebuffer size callback
	// Framebuffer size is stored in the class's height_ and width_ members

	// 1. Query the initial framebuffer size **once** after the window is created.
	//    This accounts for HiDPI scaling where physical pixels ≠ window width/height
	glfwGetFramebufferSize(window_, &width_, &height_);
	// Remember initial windowed placement/size for restoring from fullscreen
	glfwGetWindowPos(window_, &windowedX_, &windowedY_);
	glfwGetWindowSize(window_, &windowedW_, &windowedH_);

	// 2. Store `this` pointer in the GLFW window so any GLFW callback
	//    can retrieve the owning `Window` object later.
	glfwSetWindowUserPointer(window_, this);

	// 3. Register a framebuffer-resize callback.
	//    Signature required by GLFW:
	//        void callback(GLFWwindow* win, int newW, int newH);
	glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* win, int new_w, int new_h) {
		// 3-a. Recover the `Window*` that owns this GLFWwindow.
		//      glfwGetWindowUserPointer returns the same opaque void* we stored.
		auto self = static_cast<Window*>(glfwGetWindowUserPointer(win));

		// 3-b. Persist the new size inside the object so the rest of the engine
		//      can query `window.size()` without calling GLFW.
		self->width_ = new_w;
		self->height_ = new_h;

		// 3-c. Resize the OpenGL viewport to match the new framebuffer size,
		//      so that OpenGL renders to the entire window.
		//      0, 0 is the lower-left corner of the window, and new_w, new_h
		//      are the new width and height of the framebuffer.
		glViewport(0, 0, new_w, new_h);
		// std::cout << "[Window] Framebuffer resized to " << new_w << "x" << new_h << "." << std::endl;
	});

	// Window creation successful
	std::cout << "[Window] GLFW window created successfully." << std::endl;
	return true;
}

void Window::destroy() {
	if (window_) {
		glfwDestroyWindow(window_);
		window_ = nullptr;
	}
	if (glfwInitialized_) {
		glfwTerminate();
		glfwInitialized_ = false;
	}
}

void Window::setFullscreen(bool enable) {
	if (!window_) return;
	if (enable == isFullscreen_) return;
	isFullscreen_ = enable;

	if (enable) {
		// Save current windowed position and size
		glfwGetWindowPos(window_, &windowedX_, &windowedY_);
		glfwGetWindowSize(window_, &windowedW_, &windowedH_);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if (monitor && mode) {
			// Switch to true fullscreen on the primary monitor
			glfwSetWindowMonitor(window_, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
	} else {
		// Restore windowed mode at the previous position/size
		glfwSetWindowMonitor(window_, nullptr, windowedX_, windowedY_,
							 windowedW_ > 0 ? windowedW_ : 1280,
							 windowedH_ > 0 ? windowedH_ : 720,
							 0);
	}

	// Re-apply vsync and refresh cached framebuffer size/viewport
	glfwSwapInterval(1);
	glfwGetFramebufferSize(window_, &width_, &height_);
	glViewport(0, 0, width_, height_);
}

void Window::toggleFullscreen() {
	setFullscreen(!isFullscreen_);
}