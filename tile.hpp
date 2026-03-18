#pragma once

enum Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

enum Tile {
    ICE,
    WALL,
    THICK,
    WATER,
    GOAL,
    LOCK,
    HOLE,
    TELEPORTER
};

bool is_tile_moveable(Tile tile);