#include "debug.hpp"

bool g_debugEnabled = false; // Global debug flag

void toggleDebugMode() {
	g_debugEnabled = !g_debugEnabled;
	if (g_debugEnabled) {
		std::cout << "[Debug] Debug mode enabled.\n";
	} else {
		std::cout << "[Debug] Debug mode disabled.\n";
	}
}