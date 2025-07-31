// board.cpp
#include "board.hpp"

// Board Board::clone() const {
//     // יוצרים עותק עצמאי של תמונת הלוח
      

//     auto cloned_img = img->clone();  
//     return Board(cell_H_pix, cell_W_pix, W_cells, H_cells, cloned_img);
// }

Board Board::clone() const {
    auto cloned_img = img->clone();
    auto cloned_original_img = original_img->clone();

    Board new_board(cell_H_pix, cell_W_pix, W_cells, H_cells, cloned_img);
    new_board.original_img = cloned_original_img;

    // שיכפול כל החיילים שעל הלוח, אם רוצים לשמר גם אותם:
    for (const auto& [cell, piece] : pieces_on_board) {
        if (piece) {
            new_board.pieces_on_board = pieces_on_board;  // בהנחה שלפיסות יש clone()
        }
    }

    return new_board;
}


// std::pair<int, int> Board::cell_to_pixels(const Cell& cell) const {
//     int x = cell.col * 100;
//     int y = cell.row * 100;//לבדוק !!!
//     return {x, y};
// }

// std::pair<int, int> Board::cell_to_pixels(const Cell& cell) const {
//     int x = offset_x + cell.col * cell_W_pix;
//     int y = offset_y + cell.row * cell_H_pix;
//     return {x, y};
// }
// ב-board.cpp
std::pair<int, int> Board::cell_to_pixels(const Cell& cell) const {

    int x = offset_x + cell.col * cell_W_pix;
    int y = offset_y + cell.row * cell_H_pix;
    
    return {x, y};
}

