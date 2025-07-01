#include "playerobject.hpp"
#include "debug.hpp"

bool PlayerObject::tileCollision(Tilemap& tilemap, const Sensor& sensor) {
    glm::ivec2 tileIndex = tilemap.worldToTileIndex(sensor.position);
    bool isSolid = tilemap.isSolidTile(tileIndex.x, tileIndex.y);
    return isSolid; // Check if the tile at the sensor's position is solid
    // return tilemap.isSolidTile(tileIndex.x, tileIndex.y);
}