// cell.hpp

#include <cstddef>    
#include <functional> 
#pragma once

struct Cell {
    int row;
    int col;

    Cell(int r = 0, int c = 0) : row(r), col(c) {}
    
    bool operator==(const Cell& other) const {
        return row == other.row && col == other.col;
    }

    bool operator!=(const Cell& other) const {
        return !(*this == other);
    }

        bool operator<(const Cell& other) const {
        if (row != other.row) 
            return row < other.row;
        return col < other.col;
    }

        struct Hash {
        std::size_t operator()(const Cell& c) const noexcept {
            // פשוט שילוב של השדות בעזרת XOR ו-shift
            return std::hash<int>{}(c.row) ^ (std::hash<int>{}(c.col) << 1);
        }
    };
};
