#include "tilemap.hpp"
#include "debug.hpp"

Tilemap::Tilemap(int width, int height, float tilesize)
    : width_(width), height_(height), tilesize_(tilesize){

    tiles.resize(height_, std::vector<Tile>(width_));
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            // Initialize empty
            tiles[y][x].position = tileIndexToWorldPos(x, y);
            tiles[y][x].tileType = { TileEnum::EMPTY, glm::vec4(0.0f) };
        }
    }
}

Tile& Tilemap::getTile(int x, int y) {
    return tiles[y][x];
}

bool Tilemap::isSolidTile(int x, int y) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return false;
    return tiles[y][x].tileType.type == TileEnum::SOLID;
}

void Tilemap::setTile(int x, int y, const TileType& tileType) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
    tiles[y][x].tileType = tileType;
}

void Tilemap::renderTileMap(Shader& shader, Renderer2D& renderer) const {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            const Tile& tile = tiles[y][x];

            if(g_debug_enabled){
                glm::vec2 tileCenter = tile.position + glm::vec2(tilesize_ / 2.0f, tilesize_ / 2.0f);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(tileCenter, 0.0f));
                model = glm::scale(model, glm::vec3(tilesize_, tilesize_, 1.0f));
                renderer.drawQuad(shader, model, tile.tileType.color);
            }
            if (tile.tileType.type != TileEnum::EMPTY) {
                glm::vec2 tileCenter = tile.position + glm::vec2(tilesize_ / 2.0f, tilesize_ / 2.0f);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(tileCenter, 0.0f));
                model = glm::scale(model, glm::vec3(tilesize_, tilesize_, 1.0f));
                renderer.drawQuad(shader, model, tile.tileType.color);
            }
        }
    }
}

glm::ivec2 Tilemap::worldToTileIndex(const glm::vec2& pos) const {
    int x = static_cast<int>(std::floor(pos.x / tilesize_));
    int y = static_cast<int>(std::floor(pos.y / tilesize_));
    return glm::ivec2(x, y);
}

glm::vec2 Tilemap::tileIndexToWorldPos(int x, int y) const {
    return glm::vec2(x * tilesize_, y * tilesize_);
}

Tilemap loadTilemapFromFile(const std::string& filename, float tilesize) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open tilemap file: " << filename << std::endl;
        throw std::runtime_error("Failed to open tilemap file");
    }

    int width, height;
    file >> width >> height;

    std::string line;
    std::getline(file, line); // Consume the newline after height

    Tilemap tilemap(width, height, tilesize);

    for (int y = height - 1; y >= 0; --y) {
        std::getline(file, line);
        for (int x = 0; x < width && x < static_cast<int>(line.size()); ++x) {
            char c = line[x];
            TileType type;

            switch (c) {
                case '#': // Solid tile
                    type = { TileEnum::SOLID, glm::vec4(0.3f, 0.3f, 0.1f, 1.0f) };
                    break;
                case '.': // Empty tile
                default:
                    type = { TileEnum::EMPTY, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f) };
                    break;
                case 'P': // Player start position
                    type = { TileEnum::EMPTY, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f) }; // Green for player
                    tilemap.setPlayerPosition(x, y); // Set player position
                    break;
            }

            tilemap.setTile(x, y, type);
        }
    }

    return tilemap;
}
