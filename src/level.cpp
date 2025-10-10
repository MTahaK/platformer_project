#include "level.hpp"

LevelManager::LevelManager(const std::string &levelsDir) : levelsDir_(levelsDir) {
	if (!std::filesystem::exists(levelsDir_)) {
		std::cerr << "Levels directory does not exist: " << levelsDir_ << std::endl;
	}
}

bool LevelManager::extractMetadata(const fs::directory_entry& entry, LevelMetaData& metadata){
	// Assumes file at entry.path():
	// - exists
	// - is a regular file
	// - has a .tmap extension
	metadata.filepath = entry.path();
	metadata.filename = metadata.filepath.filename().string();
	metadata.displayName = metadata.filepath.filename().string();

	// Extract dimensions from .tmap file (first two lines)
	std::ifstream file(metadata.filepath.string());
	if (!file.is_open()) {
		std::cerr << "Failed to open level file: " << metadata.filepath << std::endl;
		return false;
	}

	file >> metadata.dimensions.x >> metadata.dimensions.y;

	return true;
}

bool LevelManager::loadLevelList() {
	// Iterates over directory provided in constructor, extracts metadata, and populates level options
	for (const auto& entry : fs::directory_iterator(levelsDir_)) {
		if (entry.is_regular_file() && entry.path().extension() == ".tmap") {
			LevelMetaData metadata;
			if (extractMetadata(entry, metadata)) {
				availableLevels_.push_back(metadata);
			}
		}
	}
	return !availableLevels_.empty();
	// false if no levels found, true if level list populated
}

std::vector<LevelMetaData>& LevelManager::getAvailableLevels() {
	return availableLevels_;
}

Tilemap LevelManager::loadLevel(int index) {
	if (index < 0 || index >= static_cast<int>(availableLevels_.size())) {
		throw std::out_of_range("Invalid level index");
	}
	auto& metadata = availableLevels_[index];
	return loadTilemapFromFile(metadata.filepath.string(), 1.0f, floorTex_, wallTex_);
}