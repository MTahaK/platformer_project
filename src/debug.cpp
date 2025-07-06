#include "debug.hpp"

bool g_debug_enabled = false; // Global debug flag

void toggleDebugMode(){
    g_debug_enabled = !g_debug_enabled;
    if (g_debug_enabled) {
        std::cout<<"[Debug] Debug mode enabled.\n";
    } else {
        std::cout<<"[Debug] Debug mode disabled.\n";
    }
}