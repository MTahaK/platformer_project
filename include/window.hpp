#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

// GLFW Window class
class Window {
    public:
        Window(int width, int height, const std::string& title){
            createWindow(width, height, title);
        };
        ~Window() { destroy(); }

        bool createWindow(int width, int height, const std::string& title);
        void destroy();
        
        GLFWwindow* getWindow() const { return window_; }
        
        void pollEvents() {
            glfwPollEvents();
        }
        void swap() {
            glfwSwapBuffers(window_);
        }
        void getFramebufferSize(int& width, int& height) const {
            width = width_;
            height = height_;
        }
        bool shouldClose() const {
            return glfwWindowShouldClose(window_);
        }

        void setShouldClose(bool value) {
            glfwSetWindowShouldClose(window_, value);
        }
        

    private:
        GLFWwindow* window_ = nullptr;
        int width_ = 0;
        int height_ = 0;
        static bool glfwInitialized_;
};