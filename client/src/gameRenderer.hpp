#pragma once
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <memory>
#include "../../common/src/board.hpp"
#include "../../common/src/piece.hpp"
#include "../../common/src/cell.hpp"
#include "scoreBoard.hpp"
#include "moveList.hpp"
#include "../../common/src/gameMessages.hpp"

class GameRenderer {
public:
    GameRenderer();
    void render(const std::unordered_map<std::string, std::shared_ptr<Piece>>& pieces,
                Board& board);
    void set_selected_cells(const Cell& p1_cell, const Cell& p2_cell);
    void set_command_success(bool success);
    
private:
    void _draw(const std::unordered_map<std::string, std::shared_ptr<Piece>>& pieces,
               Board& board);
    void _draw_game_info(cv::Mat& mat);
    
    Cell selected_cell_p1 {0,0};
    Cell selected_cell_p2 {7,7};
    bool command_success_ = true;
    
    ScoreBoard score_board;
    MoveList move_list;
    GameMessages game_messages;
};
