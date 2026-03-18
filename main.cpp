#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <chrono>
#include "game.hpp"
#include "parser.hpp"

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
            if (can_move && new_game.evaluate()) {

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
    auto start_time = std::chrono::steady_clock::now();

    // step 1: loading data from file given as argument
    if (argc != 2) {
        std::cout << "The program takes only one argument" << std::endl;
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

    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> run_time = end_time - start_time;
    std::cout << "Computation completed" << std::endl;
    std::cout << "Solutions found: " << i << std::endl;
    std::cout << "Elapsed time: " << run_time.count() << "s" << std::endl; 

    return 0;
}