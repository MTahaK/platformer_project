#pragma once
#include <iostream>

extern bool g_debugEnabled;

#define DEBUG_ONLY(code) do { if (g_debugEnabled) { code; } } while(0)

void toggleDebugMode();