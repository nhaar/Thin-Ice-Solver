#pragma once
#include "tile.hpp"

class Coord {
    int _x;
    int _y;
public:
    Coord(): _x{0}, _y{0} {}
    Coord(int x, int y): _x{x}, _y{y} {}

    bool is_equal(Coord& other);

    int get_x();

    int get_y();

    void set(int x, int y);

    void advance_in_direction(Direction direction);

    int get_index(int width);
};

