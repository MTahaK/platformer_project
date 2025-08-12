#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "tilemap.hpp"

// Pre-level-load, load in just level metadata for display
struct LevelMetaData {
	std::string filename;
	std::string displayName;
	glm::ivec2 dimensions; // First lines of tilemap file gives dims
	// Levels sorted by filename
};

class LevelManager {

	public:
		LevelManager(const std::string &levelsDir);

		// Level system follows 4 phase approach:
		// 1. Pre-load: Gather metadata (Discovery)
		// 2. System displays level selections (Display)
		// 3. Player makes selection (Select).
		// 4. Load the selected level after confirmation (Load)

		// When the player initially selects a level, currLevelIndex_ is set. Then, when the player confirms,
		// load the level at currLevelIndex_.

		// Phase 1: Discovery
		bool extractMetadata(const std::string& filepath, LevelMetaData& metadata);
		std::string generateDisplayName(const std::string& filename);
		bool loadLevelSelection(); // Iterates over directory structure, extracts metadata, and populates level options

		// Phase 2: Display (for UI)
		std::vector<LevelMetaData>& getAvailableLevels(); // Returned vector is then given to UI engine to display levels

		// Phase 3: Player makes selection
		bool hasSelection() const { return currLevelIndex_ >= 0; }
		void selectLevel(int index) { currLevelIndex_ = index; } // Sets current level index
		int getSelectedLevelIndex() const { return currLevelIndex_; }
		const LevelMetaData& getSelectedLevelMetaData() const;
		void clearSelection() { currLevelIndex_ = -1; }

		// Phase 4: Load after confirmation, to be run after confirmation button is pressed
		Tilemap loadSelectedLevel();

	private:
		std::vector<LevelMetaData> availableLevels_;
		std::string levelsDir_;
		int currLevelIndex_ = -1; // Select within availableLevels_
};