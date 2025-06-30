#pragma once

extern bool g_debug_enabled;

#define DEBUG_ONLY(code) do { if (g_debug_enabled) { code; } } while(0)