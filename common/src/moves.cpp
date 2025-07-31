#include "Moves.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include "piece.hpp"
#include <algorithm>
Moves::Moves(const std::string& txt_path, Cell dims)
    : dims_(dims)
{
    load_rules(txt_path);
}

void Moves::load_rules(const std::string& txt_path) {
    std::ifstream file(txt_path);
    if (!file) {
        std::cerr << "Error opening moves file: " << txt_path << std::endl;
        return;
    }

    moves_rules_.clear();

    std::string line;
    while (std::getline(file, line)) {
        // החלף פסיקים ברווחים כדי שהקריאה תעבוד
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream iss(line);
        int dr, dc;
        if (iss >> dr >> dc) {
            moves_rules_.emplace_back(dr, dc);
        }
    }
}

bool Moves::is_valid(Command cmd, const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece) const {
    // נבדוק קודם כל אם הפקודה היא מסוג Move
    if (cmd.type != "move") {
        return true;  // רק פקודות תזוזה נבדקות כאן
    }

    // נחשב את ההפרש בין יעד למקור
    int dr = cmd.dst.row - cmd.src.row;
    int dc = cmd.dst.col - cmd.src.col;
    Cell delta(dr, dc);

    // נבדוק אם ההפרש הזה קיים ברשימת החוקים
    bool found = false;
    for (const Cell& move : moves_rules_) {
        if (move == delta) {
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }

    // אם מדובר בפרש, אין צורך לבדוק את המסלול
    if (cmd.piece_id[0] == 'N') {  
        return true;
    }

auto it = cell_to_piece.find(cmd.dst);
if (it != cell_to_piece.end()) {
    // יש שם חייל - נבדוק אם הוא מאותו צבע
    if (cmd.piece_id[1] == it->second->get_id()[1]) {
        return false;
    }
}

    // אם לא פרש, נבדוק אם יש חיילים בדרך
    int steps = std::max(std::abs(dr), std::abs(dc));
    int step_r = (dr == 0) ? 0 : dr / steps;
    int step_c = (dc == 0) ? 0 : dc / steps;

    Cell current = cmd.src;
    for (int i = 1; i < steps; ++i) {
        current = Cell(current.row + step_r, current.col + step_c);
        if (cell_to_piece.find(current) != cell_to_piece.end()) {
            return false;  // יש חייל בדרך
        }
    }

    return true;
}


std::vector<Cell> Moves::get_moves(int r, int c) const {
    std::vector<Cell> result;

    for (const auto& delta : moves_rules_) {
        int nr = r + delta.row;
        int nc = c + delta.col;

        // בדיקת גבולות הלוח
        if (nr >= 0 && nr < dims_.row &&
            nc >= 0 && nc < dims_.col) {
            result.emplace_back(Cell{nr, nc});
        }
    }

    return result;
}
