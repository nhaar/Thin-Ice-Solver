#pragma once
#include <vector>

class Grid {
private:
    std::vector<int> _grid;
    int _width;
    int _height;

    int index(int x, int y);
public:
    Grid(int w, int h, std::vector<int> grid) : _width{w}, _height{h}, _grid{grid} {}

    int at(int x, int y);

    int at(int i);

    void set(int x, int y, int value);

    void set(int index, int value);

    int get_width();

    int get_height();
};

bool is_only_one_island(Grid& grid);