#pragma once
#include <iostream>

extern bool g_debug_enabled;

#define DEBUG_ONLY(code) do { if (g_debug_enabled) { code; } } while(0)

void toggleDebugMode(){
    g_debug_enabled = !g_debug_enabled;
    if (g_debug_enabled) {
        std::cout<<"[Debug] Debug mode enabled.\n";
    } else {
        std::cout<<"Debug] Debug mode disabled.\n";
    }
}