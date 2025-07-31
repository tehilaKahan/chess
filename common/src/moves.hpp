#pragma once

#include "cell.hpp"  // במקום pair
#include <vector>
#include <string>
#include "command.hpp"
class Piece;
class Moves {
public:
    Moves(const std::string& txt_path, Cell dims);

    std::vector<Cell> get_moves(int r, int c) const;
    bool is_valid(Command cmd,const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece) const;
private:
    Cell dims_;
    std::vector<Cell> moves_rules_;

    void load_rules(const std::string& txt_path);
};
