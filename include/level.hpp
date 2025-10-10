#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "debug.hpp"
#include "tilemap.hpp"
#include "texture.hpp"

namespace fs = std::filesystem;

// Pre-level-load, load in just level metadata for display
struct LevelMetaData {
	fs::path filepath; // Full path to file
	std::string filename;
	std::string displayName;
	glm::ivec2 dimensions; // First lines of tilemap file gives dims
	// Levels sorted by filename
};

class LevelManager {

	public:
		LevelManager(const std::string &levelsDir);

		bool setLevelDir(const std::string &levelsDir);
		// Level system follows 4 phase approach:
		// 1. Pre-load: Gather metadata (Discovery)
		// 2. System displays level selections (Display)
		// 3. Player makes selection (Select).
		// 4. Load the selected level after confirmation (Load)

		// Phase 1: Discovery
		bool extractMetadata(const fs::directory_entry& entry, LevelMetaData& metadata);
		std::string generateDisplayName(const std::string& filename); // May be unused
		bool loadLevelList(); // Iterates over directory structure, extracts metadata, and populates level options

		// Phase 2: Display (for UI)
		std::vector<LevelMetaData>& getAvailableLevels(); // Returned vector is then given to UI engine to display levels

		// Phase 3: Player makes selection (handled by ImGui)

		// Phase 4: Load after confirmation, to be run after confirmation button is pressed
		Tilemap loadLevel(int index); // Load level at specific index

		void clearLevelList();
		void refreshLevelList();

		Texture* wallTex_;
		Texture* floorTex_;
	private:
		std::vector<LevelMetaData> availableLevels_;
		std::string levelsDir_;
};