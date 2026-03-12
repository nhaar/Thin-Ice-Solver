#include <vector>
#include <array>
#include <iostream>
#include <fstream>

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

class Coord {
private:
    int _x;
    int _y;

public:
    Coord(): _x{0}, _y{0} {}
    Coord(int x, int y): _x{x}, _y{y} {}

    bool is_equal(Coord& other) {
        return _x == other.get_x() && _y == other.get_y();
    }

    int get_x() {
        return _x;
    }

    int get_y() {
        return _y;
    }

    void set(int x, int y) {
        _x = x;
        _y = y;
    }

    void advance_in_direction(Direction direction) {
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
};

using Solution = std::vector<Direction>;

bool is_tile_moveable(Tile tile) {
    return !(tile == Tile::WALL || tile == Tile::WATER || tile == Tile::LOCK);
}

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

    int get_index(int x, int y) {
        return _width * y + x;
    }
    
    Tile get_tile(int x, int y) {
        return _tiles.at(get_index(x, y));
    }

    void change_tile(int x, int y, Tile tile) {
        _tiles.at(get_index(x, y)) = tile;
    }

    void melt_tile(Coord new_pos) {
        Tile tile = get_tile(_puffle_pos.get_x(), _puffle_pos.get_y());
        if (tile == Tile::THICK) {
            change_tile(_puffle_pos.get_x(), _puffle_pos.get_y(), Tile::ICE);
        } else if (tile == Tile::ICE) {
            change_tile(_puffle_pos.get_x(), _puffle_pos.get_y(), Tile::WATER);
        }
        _puffle_pos = new_pos;
    }

    Coord find_other_portal(Coord this_portal) {
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

    void remove_lock(Coord pos) {
        change_tile(pos.get_x(), pos.get_y(), Tile::ICE);
    }
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

    bool is_solved() {
        for (Tile tile : _tiles) {
            if (tile == Tile::ICE || tile == Tile::THICK || tile == Tile::LOCK) {
                return false;
            }
        }
        return true;
    }

    bool apply_move(Direction direction) {
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

    Solution get_moves() {
        return _moves;
    }
};

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

    char get_char() {
        return _data[_index];
    }

public:
    DataParser(std::string data) : _data{data} {

    }

    Coord parse_coord(std::string data) {
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

    std::vector<Tile> parse_tiles(std::string data) {
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

    void parse_line() {
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

    Game parse() {
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
};

std::vector<Solution> find_solutions(Game game) {
    if (game.is_solved()) {
        return {game.get_moves()};
    } else {
        std::vector<Solution> solutions = {};
        std::array<Direction, 4> directions = {
            Direction::UP,
            Direction::RIGHT,
            Direction::DOWN,
            Direction::LEFT
        };
        for (Direction d : directions) {
            Game new_game = game;
            bool can_move = new_game.apply_move(d);
            if (can_move) {

                std::vector<Solution> found_sols = find_solutions(new_game);
                for (Solution s : found_sols) {
                    solutions.push_back(s);
                }
            }
        }
        return solutions;
    }
}

int main(int argc, char* argv[]) {
    // step 1: loading data from file given as argument
    if (argc != 2) {
        return 1;
    }
    std::ifstream file(argv[1]);
    std::string input_file  { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

    DataParser parser = DataParser(input_file);

    // step 2: create the base game that will be unsolved
    Game start = parser.parse();

    std::vector<Solution> found_solutions = find_solutions(start);
    // step 3: output all the solutions
    std::ofstream out("output.txt");
    int i = 0;
    for (std::vector<Direction> solution : found_solutions) {
        i += 1;
        out << "Solution #" << i << ": ";
        for (Direction direction : solution) {
            std::string name;
            switch (direction) {
                case Direction::UP:
                    name = "U";
                    break;
                case Direction::DOWN:
                    name = "D";
                    break;
                case Direction::LEFT:
                    name = "L";
                    break;
                case Direction::RIGHT:
                    name = "R";
                    break;
            }
            out << name;
        }
        out << std::endl;
    }

    return 0;
}