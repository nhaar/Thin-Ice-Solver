#pragma once
#include <stdexcept>
#include <vector>
#include <string>
#include "coord.hpp"
#include "game.hpp"

class DataParser {
private:
    int _index = 0;
    int _width = 0;
    int _height = 0;
    bool _spawn_set = false;
    bool _key_set = false;
    bool _block_set = false;
    bool _tiles_set = false;
    Coord _spawn;
    Coord _key;
    Coord _block;
    std::vector<Tile> _tiles;
    std::string _data;

    char get_char();

public:
    DataParser(std::string data) : _data{data} {

    }

    Coord parse_coord(std::string data);

    std::vector<Tile> parse_tiles(std::string data);

    void parse_line();

    Game parse();
};
