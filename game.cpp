#include "vector"
#include "tile.hpp"
#include "coord.hpp"
#include "stdexcept"
#include "game.hpp"
#include "grid.hpp"
#include <array>

int Game::get_index(int x, int y) {
    return _width * y + x;
}

Tile Game::get_tile(int x, int y) {
    return _tiles.at(get_index(x, y));
}

void Game::change_tile(int x, int y, Tile tile) {
    _tiles.at(get_index(x, y)) = tile;
}

void Game::melt_tile(Coord new_pos) {
    Tile tile = get_tile(_puffle_pos.get_x(), _puffle_pos.get_y());
    if (tile == Tile::THICK) {
        change_tile(_puffle_pos.get_x(), _puffle_pos.get_y(), Tile::ICE);
    } else if (tile == Tile::ICE) {
        change_tile(_puffle_pos.get_x(), _puffle_pos.get_y(), Tile::WATER);
    }
    _puffle_pos = new_pos;
}

Coord Game::find_other_portal(Coord this_portal) {
    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            Tile tile = get_tile(x, y);
            Coord coord = Coord(x, y);
            if (tile == Tile::TELEPORTER && !this_portal.is_equal(coord)) {
                return coord;
            }
        }
    }
    throw std::runtime_error("Could not find other portal");
}

void Game::remove_lock(Coord pos) {
    change_tile(pos.get_x(), pos.get_y(), Tile::ICE);
}

bool Game::has_dead_end() {
    for (int i = 0; i < _width * _height; i++) {
        // the tile the puffle is in isn't a dead end
        if (_puffle_pos.get_index(_width) == i) {
            continue;
        }
        Tile tile = _tiles.at(i);

        // a dead end is either a Hole, Ice, Lock or Thick Ice
        // surrounded by 3 walls (water or wall)
        // and neighbor to one singular ice tile
        if (!(tile == Tile::HOLE || tile == Tile::ICE || tile == Tile::THICK || tile == Tile::LOCK)) {
            continue;
        }
        int escapes = 0;
        bool adjacent_to_ice = false;
        std::array<int, 4> neighbors = {
            i - 1, // left
            i + 1, // right
            i - _width, // up
            i + _width // down
        };
        int max = _width * _height;
        for (int n : neighbors) {
            if (n > 0 && n < max) {
                Tile tile = _tiles.at(n);
                if (tile != Tile::WALL && tile != Tile::WATER) {
                    if (tile == Tile::ICE) {
                        adjacent_to_ice = true;
                    }
                    escapes++;
                }
                if (escapes > 1) {
                    break;
                }
            }
        }
        if (adjacent_to_ice && escapes == 1) {
            return true;
        }
    }
    return false;
}

bool Game::is_solved() {
    for (Tile tile : _tiles) {
        if (tile == Tile::ICE || tile == Tile::THICK || tile == Tile::LOCK) {
            return false;
        }
    }
    return true;
}

bool Game::apply_move(Direction direction) {
    _moves.push_back(direction);
    Coord new_pos = _puffle_pos;
    new_pos.advance_in_direction(direction);
    // check for block
    if (_has_block && _block_pos.is_equal(new_pos)) {
        Coord new_block_pos = _block_pos;
        Coord final_block_pos = new_block_pos;
        new_block_pos.advance_in_direction(direction);
        Tile block_tile = get_tile(new_block_pos.get_x(), new_block_pos.get_y());
        bool moved = false;
        while (is_tile_moveable(block_tile)) {
            final_block_pos = new_block_pos;
            moved = true;
            new_block_pos.advance_in_direction(direction);
            block_tile = get_tile(new_block_pos.get_x(), new_block_pos.get_y());
        }
        if (moved) {
            _block_pos = final_block_pos;
            melt_tile(new_pos);
        }
        return true;
    } else {
        Tile tile = get_tile(new_pos.get_x(), new_pos.get_y());
        if (is_tile_moveable(tile)) {
            melt_tile(new_pos);
            if (tile == Tile::TELEPORTER && !_has_teleported) {
                Coord destination = find_other_portal(new_pos);
                _puffle_pos = destination;
            }
            if (_has_key && !_picked_key && _key_pos.is_equal(new_pos)) {
                _picked_key = true;
            }
            return true;
        } else if (tile == Tile::LOCK && _picked_key) {
            _picked_key = false;
            remove_lock(new_pos);
            melt_tile(new_pos);
            return true;
        }
    }

    return false;
}

Solution Game::get_moves() {
    return _moves;
}

// function to evaluate if a given position can be solved or not
bool Game::evaluate() {
    std::vector<int> tiles = {};
    for (int i = 0; i < _height * _width; i++) {
        Tile tile = _tiles.at(i);
        if (tile == Tile::WATER || tile == Tile::WALL) {
            tiles.push_back(1);
        } else {
            tiles.push_back(0);
        }
    }
    Grid grid = Grid(_width, _height, tiles);
    return is_only_one_island(grid) && !has_dead_end();
}
