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
    HOLE,
    TELEPORTER
};

bool is_tile_moveable(Tile tile) {
    return !(tile == Tile::WALL || tile == Tile::WATER || tile == Tile::LOCK);
}

enum Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
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

class LevelMap {
private:
    int _width;
    int _height;
    bool _has_key;
    bool _has_block;
    bool _has_teleported = false;
    bool _picked_key = false;
    Coord _key_pos;
    Coord _block_pos;
    Coord _puffle_pos;
    std::vector<Tile> _tiles;
    int get_index(int x, int y) {
        return _width * y + x;
    }
public:
    LevelMap(int width, int height, bool has_key, bool has_block, Coord key_pos, Coord block_pos, Coord puffle_pos, std::vector<Tile> tiles) :
        _width{width},
        _height{height},
        _has_key{has_key},
        _has_block{has_block},
        _key_pos{key_pos},
        _block_pos{block_pos},
        _puffle_pos{puffle_pos},
        _tiles{tiles} {}

    Tile get_tile(int x, int y) {
        return _tiles.at(get_index(x, y));
    }

    std::vector<Tile> get_tiles() {
        return _tiles;
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

    void remove_lock(Coord pos) {
        change_tile(pos.get_x(), pos.get_y(), Tile::ICE);
    }

    void change_tile(int x, int y, Tile tile) {
        _tiles.at(get_index(x, y)) = tile;
    }

    int get_width() {
        return _width;
    }

    int get_height() {
        return _height;
    }

    Coord get_puffle_pos() {
        return _puffle_pos;
    }

    bool block_exists() {
        return _has_block;
    }

    Coord get_block_pos() {
        return _block_pos;
    }

    void set_puffle_pos(int x, int y) {
        _puffle_pos.set(x, y);
    }

    void set_block_pos(Coord pos) {
        _block_pos = pos;
    }

    bool has_teleported() {
        return _has_teleported;
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

    Coord get_key_pos() {
        return _key_pos;
    }

    bool has_key() {
        return _has_key;
    }

    bool picked_key() {
        return _picked_key;
    }

    void pick_key() {
        _picked_key = true;
    }

    void use_key() {
        _picked_key = false;
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
        Coord pos = _level.get_puffle_pos();
        pos.advance_in_direction(direction);
        // check for block
        if (_level.block_exists() && (_level.get_block_pos()).is_equal(pos)) {
            Coord new_block_pos = _level.get_block_pos();
            Coord final_block_pos = new_block_pos;
            new_block_pos.advance_in_direction(direction);
            Tile block_tile = _level.get_tile(new_block_pos.get_x(), new_block_pos.get_y());
            bool moved = false;
            while (is_tile_moveable(block_tile)) {
                final_block_pos = new_block_pos;
                moved = true;
                new_block_pos.advance_in_direction(direction);
                block_tile = _level.get_tile(new_block_pos.get_x(), new_block_pos.get_y());
            }
            if (moved) {
                _level.set_block_pos(final_block_pos);
                _level.melt_tile(pos);
            }
            return true;
        } else {
            Tile tile = _level.get_tile(pos.get_x(), pos.get_y());
            if (is_tile_moveable(tile)) {
                _level.melt_tile(pos);
                if (tile == Tile::TELEPORTER && !_level.has_teleported()) {
                    Coord destination = _level.find_other_portal(pos);
                    _level.set_puffle_pos(destination.get_x(), destination.get_y());
                }
                if (_level.has_key() && !_level.picked_key() && _level.get_key_pos().is_equal(pos)) {
                    _level.pick_key();
                }
                return true;
            } else if (tile == Tile::LOCK && _level.picked_key()) {
                _level.use_key();
                _level.remove_lock(pos);
                _level.melt_tile(pos);
                return true;
            }
        }

        return false;
    }
    bool is_solved() {
        for (Tile tile : _level.get_tiles()) {
            if (tile == Tile::ICE || tile == Tile::THICK || tile == Tile::LOCK) {
                return false;
            }
        }
        return true;
    }
    Game(LevelMap map) : _level{map} {

    }
};

class BackwardsGame {
private:
    LevelMap* _original_level;
    LevelMap _level;
    std::vector<Direction> _directions;
    
public:
    bool is_in_spawn() {
        Coord spawn = _original_level->get_puffle_pos();
        Coord pos = _level.get_puffle_pos();
        return pos.is_equal(spawn);
    }

    void undo_water_tile(int x, int y) {
        _level.change_tile(x, y, Tile::ICE);
    }

    void undo_ice_tile(int x, int y) {
        _level.change_tile(x, y, Tile::THICK);
    }

    void add_direction(Direction direction) {
        _directions.push_back(direction);
    }

    void set_pos(int x, int y) {
        _level.set_puffle_pos(x, y);
    }

    void try_add_new_game(std::vector<BackwardsGame>& list, int x, int y, Direction direction) {
        Tile tile = _level.get_tile(x, y);
        BackwardsGame new_game = *this;
        new_game.add_direction(direction);
        new_game.set_pos(x, y);
        if (tile == Tile::WATER) {
            new_game.undo_water_tile(x, y);
            list.push_back(new_game);
        } else if (tile == Tile::ICE) {
            Tile original = _original_level->get_tile(x, y);
            if (original == Tile::THICK) {
                new_game.undo_ice_tile(x, y);
                list.push_back(new_game);
            }
        } else if (tile == Tile::HOLE) {
            list.push_back(new_game);
        }
    }

    std::vector<BackwardsGame> get_possible_backward_variations() {
        Coord pos = _level.get_puffle_pos();
        int x = pos.get_x();
        int y = pos.get_y();
        std::vector<BackwardsGame> games = {};
        try_add_new_game(games, x, y - 1, Direction::DOWN);
        try_add_new_game(games, x, y + 1, Direction::UP);
        try_add_new_game(games, x - 1, y, Direction::RIGHT);
        try_add_new_game(games, x + 1, y, Direction::LEFT);
        return games;
    }
    std::vector<Direction> get_directions() {
        return _directions;
    }
    BackwardsGame(LevelMap* original) : _original_level{original}, _level{*original} {
        for (int y = 0; y < _level.get_height(); y++) {
            for (int x = 0; x < _level.get_width(); x++) {
                Tile tile = _level.get_tile(x, y);
                if (tile == Tile::ICE || tile == Tile::THICK || tile == Tile::LOCK) {
                    _level.change_tile(x, y, Tile::WATER);
                } else if (tile == Tile::GOAL) {
                    _level.set_puffle_pos(x, y);
                }
            }
        }
    }

    BackwardsGame(LevelMap* original, LevelMap current, std::vector<Direction> directions) :
        _original_level{original}, _level{current}, _directions{directions} {}
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
        BackwardsGame(&initial_position)
    };
    std::vector<BackwardsGame> queue = {};

    // step 3: keep unsolving games until all possibilities are finished
    int step = 0;
    while (in_progress_games.size() > 0) {
        std::cout << "Iteration: " << step << std::endl;
        step++;
        std::cout << "Number of instances " << in_progress_games.size() << std::endl;
        for (BackwardsGame game : in_progress_games) {
            std::vector<BackwardsGame> derived_games = game.get_possible_backward_variations();
            if (derived_games.size() == 0 && game.is_in_spawn()) {
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
        std::reverse(solution.begin(), solution.end());
        Game game_simulation = Game(initial_position);
        bool valid = true;
        for (Direction direction : solution) {
            if (!game_simulation.advance(direction)) {
                valid = false;
                break;
            }
        }
        if (valid && game_simulation.is_solved()) {
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