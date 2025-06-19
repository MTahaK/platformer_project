#include "input.hpp"


void Input::initialize(GLFWwindow* window){
    s_window = window;
    // Might try pre-filling key map to improve performance later
}

void Input::update(){
    s_prev = s_curr; // Store previous state

    // Iterate only over tracked keys
    // Update logic for tracked keys may come if remapping added
    for(int key : tracked_keys){
        int state = glfwGetKey(s_window, key);
        // 0 -> GLFW_RELEASED, 1-> GLFW_PRESS
        s_curr[key] = state;
    }
}

bool Input::isKeyPressed(int key){
    auto it = s_curr.find(key);
    return it != s_curr.end() && it->second == GLFW_PRESS;
    // Prevents insertion of unadded keys into map
}

bool Input::isKeyJustPressed(int key){
    auto it_curr = s_curr.find(key);
    auto it_prev = s_prev.find(key);
    // Calling find() on the key prevents insertion
    if(it_curr != s_curr.end() && it_prev != s_prev.end()){
        if(it_curr->second == GLFW_PRESS && it_prev->second == GLFW_RELEASE){
            return true;
        }
    }
    return false;
}

bool Input::isKeyJustReleased(int key){
    auto it_curr = s_curr.find(key);
    auto it_prev = s_prev.find(key);
    // Calling find() on the key prevents insertion
    if(it_curr != s_curr.end() && it_prev != s_prev.end()){
        if(it_curr->second == GLFW_RELEASE && it_prev->second == GLFW_PRESS){
            return true;
        }
    }
    return false;
}