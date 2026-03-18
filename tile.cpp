#include "tile.hpp"

bool is_tile_moveable(Tile tile) {
    return !(tile == Tile::WALL || tile == Tile::WATER || tile == Tile::LOCK);
}