#include "coord.hpp"

bool Coord::is_equal(Coord& other) {
    return _x == other.get_x() && _y == other.get_y();
}

int Coord::get_x() {
    return _x;
}

int Coord::get_y() {
    return _y;
}

void Coord::set(int x, int y) {
    _x = x;
    _y = y;
}

void Coord::advance_in_direction(Direction direction) {
    switch (direction) {
        case Direction::UP:
            _y -= 1;
            break;
        case Direction::DOWN:
            _y += 1;
            break;
        case Direction::LEFT:
            _x -= 1;
            break;
        case Direction::RIGHT:
            _x += 1;
            break;
    }
}

int Coord::get_index(int width) {
    return _y * width + _x;
}