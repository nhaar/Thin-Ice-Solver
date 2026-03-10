#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <stdexcept>

enum Tile {
    ICE,
    WALL,
    THICK,
    WATER,
    GOAL,
    LOCK,
    HOLE
};

enum Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

class LevelMap {
private:
    int _width;
    int _height;
    bool _has_key;
    bool _has_block;
    bool _has_teleported;
    std::array<int, 2> _key_pos;
    std::array<int, 2> _block_pos;
    std::array<int, 2> _puffle_pos;
    std::vector<Tile> _tiles;
public:
    LevelMap(int width, int height, bool has_key, bool has_block, bool has_teleported, std::array<int, 2> key_pos, std::array<int, 2> block_pos, std::array<int, 2> puffle_pos, std::vector<Tile> tiles) :
        _width{width},
        _height{height},
        _has_key{has_key},
        _has_block{has_block},
        _has_teleported{has_teleported},
        _key_pos{key_pos},
        _block_pos{block_pos},
        _puffle_pos{puffle_pos},
        _tiles{tiles} {}

    LevelMap clone() {
        std::array<int, 2> new_key = _key_pos;
        std::array<int, 2> new_block = _block_pos;
        std::array<int, 2> new_puffle = _puffle_pos;
        std::vector<Tile> new_tile = _tiles;
        return LevelMap(
            _width,
            _height,
            _has_key,
            _has_block,
            _has_teleported,
            new_key,
            new_block,
            new_puffle,
            new_tile
        );
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
    std::array<int, 2> _spawn;
    std::array<int, 2> _key;
    std::array<int, 2> _block;
    std::vector<Tile> _tiles;
    std::string _data;

    char get_char() {
        return _data[_index];
    }

public:
    DataParser(std::string data) : _data{data} {

    }

    std::array<int, 2> parse_coord(std::string data) {
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
        return std::array<int, 2> { std::stoi(x), std::stoi(y) };
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

    LevelMap parse() {
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
        return LevelMap(
            _width,
            _height,
            _key_set,
            _block_set,
            false,
            _key,
            _block,
            _spawn,
            _tiles
        );
    }
};
class Game {
private:
    LevelMap _level;
public:
    bool advance(Direction direction) {
        return true;
    }
    bool is_solved() {
        return true;
    }
    Game(LevelMap map) : _level{map.clone()} {

    }
};

class BackwardsGame {
private:
    LevelMap _level;
    std::vector<Direction> _directions;
    
public:
    std::vector<BackwardsGame> get_possible_backward_variations() {
        return std::vector<BackwardsGame> {};
    }
    std::vector<Direction> get_directions() {
        return _directions;
    }
    BackwardsGame(LevelMap map) : _level{map.clone()} {
    }
};

int main(int argc, char* argv[]) {
    // step 1: loading data from file given as argument
    if (argc != 2) {
        return 1;
    }
    std::ifstream file(argv[1]);
    std::string input_file  { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

    DataParser parser = DataParser(input_file);

    // step 2: create the base game that will be unsolved
    LevelMap initial_position = parser.parse();

    std::vector<std::vector<Direction>> found_solutions = {};
    std::vector<BackwardsGame> in_progress_games = {
        BackwardsGame(initial_position)
    };
    std::vector<BackwardsGame> queue = {};

    // step 3: keep unsolving games until all possibilities are finished
    while (in_progress_games.size() > 0) {
        for (BackwardsGame game : in_progress_games) {
            std::vector<BackwardsGame> derived_games = game.get_possible_backward_variations();
            if (derived_games.size() == 0) {
                found_solutions.push_back(game.get_directions());
            } else {
                for (BackwardsGame derived_game : derived_games) {
                    queue.push_back(derived_game);
                }
            }
        }

        in_progress_games = queue;
        queue = {};
    }

    // step 4: check which of the solutions are valid
    std::vector<std::vector<Direction>> valid_solutions = {};
    for (std::vector<Direction> solution : found_solutions) {
        Game game_simulation = Game(initial_position);
        bool valid = true;
        for (Direction direction : solution) {
            if (!game_simulation.advance(direction)) {
                valid = false;
                break;
            }
        }
        if (game_simulation.is_solved()) {
            valid_solutions.push_back(solution);
        }
    }
    // step 5: output all the solutions
    std::ofstream out("output.txt");
    int i = 0;
    for (std::vector<Direction> solution : valid_solutions) {
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