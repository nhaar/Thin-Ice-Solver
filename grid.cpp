#include <array>
#include <stdexcept>
#include "grid.hpp"

int Grid::index(int x, int y) {
    return y * _width + x;
}

int Grid::at(int x, int y) {
    return _grid.at(index(x, y));
}

int Grid::at(int i) {
    return _grid.at(i);
}

void Grid::set(int x, int y, int value) {
    _grid.at(index(x, y)) = value;
}

void Grid::set(int index, int value) {
    _grid.at(index) = value;
}

int Grid::get_width() {
    return _width;
}

int Grid::get_height() {
    return _height;
}

// grid given can only have 0s or 1s (0 = walkable, 1 is unwalkable)
bool is_only_one_island(Grid& grid) {
    int x_0 = -1;
    int y_0 = -1;

    // checking for first block with value 0
    for (int y = 0; y < grid.get_height(); y++) {
        for (int x = 0; x < grid.get_width(); x++) {
            if (grid.at(x, y) == 0) {
                x_0 = x;
                y_0 = y;
                break;
            }
        }
        if (y_0 != -1) {
            break;
        }
    }
    if (x_0 == -1) {
        throw std::runtime_error("No walkable tile");
    }

    std::vector<int> coords = { y_0 * grid.get_width() + x_0 };
    std::vector<int> queue = {};

    int max = grid.get_height() * grid.get_width();

    // keep going in adjacent blocks until it covers all blocks touchable
    // leave their value as 2
    // there will be disconnected islands if any value is stil 0
    while (coords.size() > 0) {
        for (int coord : coords) {
            std::array<int, 4> neighbors = {
                coord - 1, // left
                coord + 1, // right
                coord - grid.get_width(), // up
                coord + grid.get_width() // down
            };
            grid.set(coord, 2);
            for (int n : neighbors) {
                if (n > 0 && n < max && grid.at(n) == 0) {
                    queue.push_back(n);
                }
            }
        }
        coords = queue;
        queue = {};
    }

    for (int y = 0; y < grid.get_height(); y++) {
        for (int x = 0; x < grid.get_width(); x++) {
            if (grid.at(x, y) == 0) {
                return false;
            }
        }
    }
    return true;
}
