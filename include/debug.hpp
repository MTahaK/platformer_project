#pragma once
#include <iostream>

extern bool g_debug_enabled;

#define DEBUG_ONLY(code) do { if (g_debug_enabled) { code; } } while(0)

void toggleDebugMode();