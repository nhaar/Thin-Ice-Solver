#pragma once
#include "vector"
#include "tile.hpp"
#include "coord.hpp"
#include "stdexcept"

using Solution = std::vector<Direction>;

class Game {
    int _width;
    int _height;
    bool _has_key;
    bool _has_block;
    bool _has_teleported = false;
    bool _picked_key = false;
    std::vector<Direction> _moves = {};
    Coord _key_pos;
    Coord _block_pos;
    Coord _puffle_pos;
    std::vector<Tile> _tiles;

    int get_index(int x, int y);
    
    Tile get_tile(int x, int y);

    void change_tile(int x, int y, Tile tile);

    void melt_tile(Coord new_pos);

    Coord find_other_portal(Coord this_portal);

    void remove_lock(Coord pos);

    bool has_dead_end();
public:
    Game(int width, int height, bool has_key, bool has_block, Coord key_pos, Coord block_pos, Coord puffle_pos, std::vector<Tile> tiles) :
        _width{width},
        _height{height},
        _has_key{has_key},
        _has_block{has_block},
        _key_pos{key_pos},
        _block_pos{block_pos},
        _puffle_pos{puffle_pos},
        _tiles{tiles} {}

    bool is_solved();

    bool apply_move(Direction direction);

    Solution get_moves();

    // function to evaluate if a given position can be solved or not
    bool evaluate();
};
