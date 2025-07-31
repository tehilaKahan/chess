#include "gameRenderer.hpp"

GameRenderer::GameRenderer() {
    // אתחול
}

void GameRenderer::render(const std::unordered_map<std::string, std::shared_ptr<Piece>>& pieces,
                         Board& board) {
    _draw(pieces, board);
    
    if (!board.img || board.img->empty()) return;
    cv::imshow("Game Board", board.img->get_mat());
}

void GameRenderer::set_selected_cells(const Cell& p1_cell, const Cell& p2_cell) {
    selected_cell_p1 = p1_cell;
    selected_cell_p2 = p2_cell;
}

void GameRenderer::set_command_success(bool success) {
    command_success_ = success;
}

void GameRenderer::_draw(const std::unordered_map<std::string, std::shared_ptr<Piece>>& pieces,
                        Board& board) {
    if (!board.img || board.img->empty()) return;
    cv::Mat& current_mat = board.img->get_mat();
    
    // יצירת רקע עם תמונת רקע אמיתית
    cv::Mat background = cv::imread("../../background.jpg");
    if (!background.empty()) {
        cv::resize(background, background, cv::Size(current_mat.cols, current_mat.rows));
        background.copyTo(current_mat);
    } else {
        current_mat.setTo(cv::Scalar(50, 50, 50));
    }
    
    // טען לוח בגודל קטן יותר
    cv::Mat board_only = cv::imread("../../board.png");

    if (!board_only.empty()) {
        cv::resize(board_only, board_only, cv::Size(700, 700));
        int offset_x = (current_mat.cols - 700) / 2;
        int offset_y = (current_mat.rows - 700) / 2;
        
        if (offset_x >= 0 && offset_y >= 0) {
            cv::Rect roi(offset_x, offset_y, 700, 700);
            board_only.copyTo(current_mat(roi));
        }
    }
    
    // ציור החיילים - שנה pieces_ ל-pieces
    for (const auto& [id, piece] : pieces) {
        if (!piece) continue;
        piece->draw_on_board(board, 0);  // הסר const אם צריך
    }
    
    // ציור ריבוע הבחירה
    int offset_x = (current_mat.cols - 700) / 2;
    int offset_y = (current_mat.rows - 700) / 2;
    int cell_size = 87;

    // סמן שחקן 1 (לבן)
    int x1 = offset_x + selected_cell_p1.col * cell_size;
    int y1 = offset_y + selected_cell_p1.row * cell_size;
    cv::rectangle(current_mat, cv::Rect(x1, y1, cell_size, cell_size), cv::Scalar(0, 255, 0), 3);
    
    // סמן שחקן 2 (שחור)
    int x2 = offset_x + selected_cell_p2.col * cell_size;
    int y2 = offset_y + selected_cell_p2.row * cell_size;
    cv::rectangle(current_mat, cv::Rect(x2, y2, cell_size, cell_size), cv::Scalar(0, 0, 255), 3);

    // הוספת הצגת ניקוד ומהלכים
    _draw_game_info(current_mat);
}

void GameRenderer::_draw_game_info(cv::Mat& mat) {
    auto scores = score_board.get_scores();
    auto recent_moves = move_list.get_last_moves(20);
    
    // מידע שחקן שחור (שמאל)
    int left_x = 10;
    int left_y = 30;
    
    cv::putText(mat, "PLAYER 2 (BLACK):", cv::Point(left_x, left_y), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
    left_y += 30;
    
    // ניקוד שחקן שחור
    int black_score_val = (scores.find("Black") != scores.end()) ? scores.at("Black") : 0;
    std::string score_text = "Score: " + std::to_string(black_score_val);
    cv::putText(mat, score_text, cv::Point(left_x, left_y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    left_y += 40;
    
    // מהלכים של שחקן שחור
    cv::putText(mat, "MOVES:", cv::Point(left_x, left_y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    left_y += 25;
    
    int black_move_count = 0;
    for (const auto& move : recent_moves) {
        if (move.find("B") != std::string::npos && black_move_count < 15) {
            cv::putText(mat, move, cv::Point(left_x, left_y), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0), 1);
            left_y += 18;
            black_move_count++;
        }
        if (left_y > mat.rows - 50) break;
    }
    
    // מידע שחקן לבן (ימין)
    int right_x = mat.cols - 250;
    int right_y = 30;
    
    cv::putText(mat, "PLAYER 1 (WHITE):", cv::Point(right_x, right_y), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
    right_y += 30;
    
    // ניקוד שחקן לבן
    int white_score_val = (scores.find("White") != scores.end()) ? scores.at("White") : 0;
    score_text = "Score: " + std::to_string(white_score_val);
    cv::putText(mat, score_text, cv::Point(right_x, right_y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    right_y += 40;
    
    // מהלכים של שחקן לבן
    cv::putText(mat, "MOVES:", cv::Point(right_x, right_y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    right_y += 25;
    
    int white_move_count = 0;
    for (const auto& move : recent_moves) {
        if (move.find("W") != std::string::npos && white_move_count < 15) {
            cv::putText(mat, move, cv::Point(right_x, right_y), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0), 1);
            right_y += 18;
            white_move_count++;
        }
        if (right_y > mat.rows - 50) break;
    }
    
    game_messages.draw_message(mat);
}
