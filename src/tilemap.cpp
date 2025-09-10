#include "tilemap.hpp"
#include "debug.hpp"

Tilemap::Tilemap(int width, int height, float tileSize) : width_(width), height_(height), tileSize_(tileSize) {

	tiles_.resize(height_, std::vector<Tile>(width_));
	for (int y = 0; y < height_; ++y) {
		for (int x = 0; x < width_; ++x) {
			// Initialize empty
			tiles_[y][x].position = tileIndexToWorldPos(x, y);
			tiles_[y][x].tileType = {TileEnum::EMPTY, false, false, glm::vec4(0.0f)};
		}
	}
}

Tile& Tilemap::getTile(int x, int y) { return tiles_[y][x]; }

bool Tilemap::isSolidTile(int x, int y) {
	if (x < 0 || x >= width_ || y < 0 || y >= height_)
		return false;
	return tiles_[y][x].tileType.type == TileEnum::SOLID ||
		   tiles_[y][x].tileType.solid == true;
}

bool Tilemap::isGoalTile(int x, int y) {
	if (x < 0 || x >= width_ || y < 0 || y >= height_)
		return false;
	return tiles_[y][x].tileType.type == TileEnum::GOAL;
}

void Tilemap::setTile(int x, int y, const TileType& tileType) {
	if (x < 0 || x >= width_ || y < 0 || y >= height_)
		return;
	tiles_[y][x].tileType = tileType;
}

void Tilemap::renderTileMap(Shader& shader, Renderer2D& renderer) const {
	for (int y = 0; y < height_; ++y) {
		for (int x = 0; x < width_; ++x) {
			const Tile& tile = tiles_[y][x];
			if (tile.tileType.visible) {
				glm::vec2 tileCenter = tile.position + glm::vec2(tileSize_ / 2.0f, tileSize_ / 2.0f);
				glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(tileCenter, 0.0f));
				model = glm::scale(model, glm::vec3(tileSize_, tileSize_, 1.0f));
				renderer.drawQuad(shader, model, tile.tileType.color);
			}
		}
	}
}

glm::ivec2 Tilemap::worldToTileIndex(const glm::vec2& pos) const {
	int x = static_cast<int>(std::floor(pos.x / tileSize_));
	int y = static_cast<int>(std::floor(pos.y / tileSize_));
	return glm::ivec2(x, y);
}

glm::vec2 Tilemap::tileIndexToWorldPos(int x, int y) const { return glm::vec2(x * tileSize_, y * tileSize_); }

Tilemap loadTilemapFromFile(const std::string& filename, float tileSize) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open tilemap file: " << filename << std::endl;
		throw std::runtime_error("Failed to open tilemap file");
	}

	int width, height;
	file >> width >> height;

	std::string line;
	std::getline(file, line); // Consume the newline after height

	Tilemap tilemap(width, height, tileSize);
	bool startSet = false, dwallStartSet = false, dwallEndSet = false;
	for (int y = height - 1; y >= 0; --y) {
		std::getline(file, line);
		for (int x = 0; x < width && x < static_cast<int>(line.size()); ++x) {
			char c = line[x];
			TileType type;

			switch (c) {
			case '#': // Solid tile
				type = {TileEnum::SOLID, true, true, glm::vec4(0.3f, 0.3f, 0.1f, 1.0f)};
				break;
			case '.': // Empty tile
			default:
				type = {TileEnum::EMPTY, false, false, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)};
				break;
			case 'P': // Player start position
				if(!startSet) {
					type = {TileEnum::PLAYER, false, false, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)};
					tilemap.setPlayerPos(x, y);
					startSet = true;
					break;
				} else {
					// Only one player start position can be set
					std::cerr << "Multiple player start positions found in tilemap file." << std::endl;
				}
			case '[':
				// Death wall starts and ends will always be within a solid 'wall' tile
				if (!dwallStartSet) {
					type = {TileEnum::DWALLSTART, true, true, glm::vec4(0.3f, 0.3f, 0.1f, 1.0f)};
					tilemap.setDeathWallStartPos(x, y);
					dwallStartSet = true;
					break;
				} else {
					std::cerr << "Multiple death wall start positions found in tilemap file." << std::endl;
				}
				
			case ']':
				if (!dwallEndSet) {
					type = {TileEnum::DWALLEND, true, true, glm::vec4(0.3f, 0.3f, 0.1f, 1.0f)};
					tilemap.setDeathWallEndPos(x, y);
					dwallEndSet = true;
				} else {
					std::cerr << "Multiple death wall end positions found in tilemap file." << std::endl;
				}
				break;
			case 'G': // Goal tile
				type = {TileEnum::GOAL, true, false, rgbaToVec4("0, 74, 20, 255")};
				tilemap.setGoalPos(x, y); // Set goal position
				break;
			}

			tilemap.setTile(x, y, type);
		}
	}
	if (!startSet || !dwallStartSet || !dwallEndSet) {
		std::cerr << "Tilemap is missing required positions." << std::endl;
		throw std::runtime_error("Tilemap is missing required positions.");
	}

	return tilemap;
}
