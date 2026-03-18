#include "parser.hpp"

char DataParser::get_char() {
    return _data[_index];
}

Coord DataParser::parse_coord(std::string data) {
    std::string x = "";
    std::string y = "";
    bool x_done = false;
    for (char c : data) {
        if (c == ',') {
            x_done = true;
        } else {
            if (x_done) {
                y += c;
            } else {
                x += c;
            }
        }
    }
    return Coord(std::stoi(x), std::stoi(y));
}

std::vector<Tile> DataParser::parse_tiles(std::string data) {
    std::vector<Tile> tiles = {};
    for (char c : data) {
        Tile tile;
        switch (c) {
            case 'I':
                tile = Tile::ICE;
                break;
            case 'W':
                tile = Tile::WALL;
                break;
            case 'T':
                tile = Tile::THICK;
                break;
            case 'G':
                tile = Tile::GOAL;
                break;
            case 'L':
                tile = Tile::LOCK;
                break;
            case 'H':
                tile = Tile::HOLE;
                break;
            case 'P':
                tile = Tile::TELEPORTER;
                break;
            default:
                throw std::runtime_error("Invalid tile type: " + c);
        }
        tiles.push_back(tile);
    }
    
    return tiles;
}

void DataParser::parse_line() {
    bool key_done = false;
    std::string key = "";
    std::string value = "";
    while (_index < _data.length()) {
        char c = _data[_index];
        if (c == '=') {
            key_done = true;
        } else if (c == '\n') {
            _index++;
            break;
        } else {
            if (key_done) {
                value += c;
            } else {
                key += c;
            }
        }
        _index++;
    }

    if (key == "width") {
        _width = std::stoi(value);
    } else if (key == "height") {
        _height = std::stoi(value);
    } else if (key == "spawn") {
        _spawn_set = true;
        _spawn = parse_coord(value);
    } else if (key == "key") {
        _key_set = true;
        _key = parse_coord(value);
    } else if (key == "block") {
        _block_set = true;
        _block = parse_coord(value);
    } else if (key == "tiles") {
        _tiles_set = true;
        _tiles = parse_tiles(value);   
    }
}

Game DataParser::parse() {
    while (_index < _data.length()) {
        parse_line();
    }
    if (!_spawn_set) {
        throw std::runtime_error("No spawn point in level");
    }
    if (!_tiles_set) {
        throw std::runtime_error("No tiles defined");
    }
    if (_width == 0 || _height == 0) {
        throw std::runtime_error("Map dimensions not set");
    }
    return Game(
        _width,
        _height,
        _key_set,
        _block_set,
        _key,
        _block,
        _spawn,
        _tiles
    );
}
