#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "shader.hpp"
#include "renderer2d.hpp"
#include "color.hpp"
#include "texture.hpp"

// Define Tile type enum
enum class TileEnum { EMPTY, SOLID, PLAYER, DWALLSTART, DWALLEND, GOAL };

struct TileType {
		TileEnum type; // Type of the tile (e.g., EMPTY, SOLID)
		bool visible;
		bool solid;
		glm::vec4 color; // Color of the tile
};

struct Tile {
		glm::vec2 position; // Position of the tile in world coordinates
		TileType tileType;	// Type and color of the tile
		Texture* texture = nullptr; // Pointer to the texture used for rendering the tile
};

class Tilemap {

	public:
		Tilemap(int width, int height, float tileSize);

		Tile& getTile(int x, int y);
		glm::ivec2 getInitPlayerPos() { return playerPos_; }
		glm::ivec2 getGoalPos() { return goalPos_; }
		bool isSolidTile(int x, int y);
		bool isGoalTile(int x, int y);

		// Setters
		void setTile(int x, int y, const TileType& tileType);			   // Set tile at position (x, y) with a specific type
		void setPlayerPos(int x, int y) { playerPos_ = glm::ivec2(x, y); } // Set initial player position in tile indices
		void setGoalPos(int x, int y) { goalPos_ = glm::ivec2(x, y); }	   // Set goal position in tile indices
		void setDeathWallStartPos(int x, int y) { deathWallStartPos_ = glm::ivec2(x, y); } // Set death wall start position in tile indices
		void setDeathWallEndPos(int x, int y) { deathWallEndPos_ = glm::ivec2(x, y); } // Set death wall end position in tile indices

		void renderTileMap(Shader& shader, Renderer2D& renderer) const; // Render the tilemap using the provided shader and renderer

		// World <-> Grid conversions
		glm::ivec2 worldToTileIndex(const glm::vec2& pos) const;
		glm::vec2 tileIndexToWorldPos(int x, int y) const;

		// Getters
		int getWidth() const { return width_; }
		int getHeight() const { return height_; }
		glm::ivec2 getPlayerPosition() const { return playerPos_; }
		glm::ivec2 getDeathWallStartPosition() const { return deathWallStartPos_; }
		glm::ivec2 getDeathWallEndPosition() const { return deathWallEndPos_; }
		float getTileSize() const { return tileSize_; }

	private:
		std::vector<std::vector<Tile>> tiles_; // 2D vector to hold tiles

		int width_;			   // Width of the tilemap in tiles
		int height_;		   // Height of the tilemap in tiles
		glm::ivec2 playerPos_; // Initial player position in tile indices
		glm::ivec2 deathWallStartPos_; // Position of the death wall start tile in tile indices
		glm::ivec2 deathWallEndPos_; // Position of the death wall end tile in tile indices - must be aligned on one axis with start
		glm::ivec2 goalPos_;   // Goal position in tile indices

		float tileSize_; // Size of one tile in world units

		// Texture* wallTex_;
		// Texture* floorTex_;
};

Tilemap loadTilemapFromFile(const std::string& filename, float tileSize, Texture* floorTex, Texture* wallTex);
