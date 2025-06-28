#include "tilemap.hpp"

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
            if (tile.tileType.type != TileEnum::EMPTY) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(tile.position, 0.0f));
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
