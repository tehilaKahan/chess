#pragma once
#include "img.hpp"
#include <utility>  // בשביל std::pair
#include "cell.hpp"
#include <map>
#include <memory>
#include "piece.hpp"


class Board {
public :
    int cell_H_pix;
    int cell_W_pix;
    int W_cells;
    int H_cells;
    int offset_x = 0;
    int offset_y = 0;

    std::shared_ptr<Img> img;
    std::shared_ptr<Img> original_img;

    std::map<Cell, std::shared_ptr<Piece>> pieces_on_board;

   Board(int cellH, int cellW, int Wcells, int Hcells, std::shared_ptr<Img> image_ptr)
    : cell_H_pix(cellH), cell_W_pix(cellW), W_cells(Wcells), H_cells(Hcells), img(image_ptr) ,original_img(std::make_shared<Img>(*image_ptr)) {}

    // Clone method - מחזיר עותק חדש של האובייקט
void clear() {
    if (img && original_img) {
        original_img->get_mat().copyTo(img->get_mat());
    }
}



    Board clone() const;
    float get_pixels_per_meter() const { return 100.0; }
    void place_piece(const std::shared_ptr<Piece>& piece) {
    pieces_on_board[piece->get_cell()] = piece;
}

    // פונקציה שממירה תא לקואורדינטות פיקסלים
    std::pair<int, int> cell_to_pixels(const Cell& cell) const;
};
