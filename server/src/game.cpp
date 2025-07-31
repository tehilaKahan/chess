#include "game.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include "background.hpp"
#include <windows.h>

using json = nlohmann::json;

Game::Game(const std::vector<std::shared_ptr<Piece>>& pieces, const Board& board)
    : board_(board)
{
    for (const auto& p : pieces) {
        pieces_[p->get_id()] = p;
    }
}

int Game::game_time_ms() const {
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_tp).count());
}

Board Game::clone_board() const {
    return board_.clone(); // assuming such method exists
}

void Game::run() {
    running_ = true;
    publisher.publish("game_started", "");
    if(websocket_callback_) {
        websocket_callback_("game_started", "");
    }
    
    start_tp = std::chrono::steady_clock::now();

    for (auto& [id, piece] : pieces_) {
        piece->reset(0);
    }

    while (running_ && !_is_win()) {
        command_success_ = true;
        int now = game_time_ms();
        auto cell_to_piece = build_cell_to_piece_map();

        for (auto& [id, piece] : pieces_) {
            piece->update(now);
        }

        // עיבוד פקודות מהתור
        {
            std::lock_guard<std::mutex> lock(command_queue_mutex);
            while (!user_input_queue.empty()) {
                Command cmd = user_input_queue.front();
                user_input_queue.pop();
                _process_input(cmd, cell_to_piece);
            }
        }
        
        _resolve_collisions();
        
        // השהיה קטנה
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    if(websocket_callback_) {
        websocket_callback_("game_won", "");
    }
    publisher.publish("game_won", "");
    _announce_win();
}




// void Game::run() {
//     if(websocket_callback_) {
//         websocket_callback_("game_started", "Game is starting");
//     }
//     publisher.publish("game_started", "");
//     start_tp = std::chrono::steady_clock::now();

//     for (auto& [id, piece] : pieces_) {
//         piece->reset(0);
//     }

//     // אתחול סמנים
//     selected_cell_p1 = {6, 0};  // מיקום עם כלי לבן
//     selected_cell_p2 = {0, 0};  // מיקום עם כלי שחור

//     auto clamp_cell = [](Cell& c) {
//         c.row = std::clamp(c.row, 0, 7);
//         c.col = std::clamp(c.col, 0, 7);
//     };

//     while (!_is_win()) {
//         command_success_ = true;
//         int now = game_time_ms();
//         auto cell_to_piece = build_cell_to_piece_map();

//         for (auto& [id, piece] : pieces_) {
//             piece->update(now);
//         }

//         while (!user_input_queue.empty()) {
//             Command cmd = user_input_queue.front();
//             user_input_queue.pop();
//             _process_input(cmd, cell_to_piece);
//         }
//         _resolve_collisions();
//         board_.clear();
//         _draw();

//         if (!board_.img || board_.img->empty()) {
//             std::cerr << "Error: Board image is empty" << std::endl;
//             break;
//         }
//         cv::imshow("Game Board", board_.img->get_mat());

//         int key = cv::waitKey(50);
//         if (key == 27) break; // ESC לסיום
//         if (key == -1) continue;

//         std::cout << "[KeyPress] key=" << key << std::endl;

//         // שחקן 1 (לבן) - תנועה (WASD)
//         if (key == 'w' || key == 'W') {
//             selected_cell_p1.row--;
//             clamp_cell(selected_cell_p1);
//             std::cout << "Player 1 moved to (" << selected_cell_p1.row << ", " << selected_cell_p1.col << ")\n";
//         } else if (key == 's' || key == 'S') {
//             selected_cell_p1.row++;
//             clamp_cell(selected_cell_p1);
//             std::cout << "Player 1 moved to (" << selected_cell_p1.row << ", " << selected_cell_p1.col << ")\n";
//         } else if (key == 'a' || key == 'A') {
//             selected_cell_p1.col--;
//             clamp_cell(selected_cell_p1);
//             std::cout << "Player 1 moved to (" << selected_cell_p1.row << ", " << selected_cell_p1.col << ")\n";
//         } else if (key == 'd' || key == 'D') {
//             selected_cell_p1.col++;
//             clamp_cell(selected_cell_p1);
//             std::cout << "Player 1 moved to (" << selected_cell_p1.row << ", " << selected_cell_p1.col << ")\n";
//         }
//         // שחקן 2 (שחור) - תנועה (IJKL)
//         else if (key == 'i' || key == 'I') {
//             selected_cell_p2.row--;
//             clamp_cell(selected_cell_p2);
//             std::cout << "Player 2 moved to (" << selected_cell_p2.row << ", " << selected_cell_p2.col << ")\n";
//         } else if (key == 'k' || key == 'K') {
//             selected_cell_p2.row++;
//             clamp_cell(selected_cell_p2);
//             std::cout << "Player 2 moved to (" << selected_cell_p2.row << ", " << selected_cell_p2.col << ")\n";
//         } else if (key == 'j' || key == 'J') {
//             selected_cell_p2.col--;
//             clamp_cell(selected_cell_p2);
//             std::cout << "Player 2 moved to (" << selected_cell_p2.row << ", " << selected_cell_p2.col << ")\n";
//         } else if (key == 'l' || key == 'L') {
//             selected_cell_p2.col++;
//             clamp_cell(selected_cell_p2);
//             std::cout << "Player 2 moved to (" << selected_cell_p2.row << ", " << selected_cell_p2.col << ")\n";
//         }
//         // שחקן 1 - בחירה (רווח)
//         else if (key == 32) {
//             std::cout << "[DEBUG] Space pressed for Player 1" << std::endl;
//             if (selecting_source_p1) {
//                 src_p1 = selected_cell_p1;
//                 selected_piece_id_p1.clear();
//                 std::cout << "[DEBUG] Looking for white piece at (" << src_p1.row << "," << src_p1.col << ")" << std::endl;
                
//                 // הדפסת כל הכלים
//                 std::cout << "[DEBUG] All pieces:" << std::endl;
//                 for (auto& [id, piece] : pieces_) {
//                     std::cout << "  Piece " << id << " at (" << piece->get_cell().row << "," << piece->get_cell().col << ")" << std::endl;
//                 }
                
//                 for (auto& [id, piece] : pieces_) {
//                     if (piece->get_cell() == src_p1 && id.length() >= 2 && id[1] == 'W') {
//                         selected_piece_id_p1 = id;
//                         std::cout << "[DEBUG] Found white piece: " << id << std::endl;
//                         break;
//                     }
//                 }
//                 if (!selected_piece_id_p1.empty()) {
//                     selecting_source_p1 = false;
//                     std::cout << "[DEBUG] Source selected, now select destination" << std::endl;
//                 } else {
//                     std::cout << "[DEBUG] No white piece found" << std::endl;
//                 }
//             } else {
//                 dst_p1 = selected_cell_p1;
//                 std::cout << "[DEBUG] Creating move command from (" << src_p1.row << "," << src_p1.col << ") to (" << dst_p1.row << "," << dst_p1.col << ")" << std::endl;
//                 Command cmd(game_time_ms(), selected_piece_id_p1, "move", src_p1, dst_p1);
//                 user_input_queue.push(cmd);
//                 selecting_source_p1 = true;
//                 std::cout << "[DEBUG] Command queued for Player 1" << std::endl;
//             }
//         }
//         // שחקן 2 - בחירה (אנטר)
//         else if (key == 13) {
//             std::cout << "[DEBUG] Enter pressed for Player 2" << std::endl;
//             if (selecting_source_p2) {
//                 src_p2 = selected_cell_p2;
//                 selected_piece_id_p2.clear();
//                 std::cout << "[DEBUG] Looking for black piece at (" << src_p2.row << "," << src_p2.col << ")" << std::endl;
//                 for (auto& [id, piece] : pieces_) {
//                     if (piece->get_cell() == src_p2 && id.length() >= 2 && id[1] == 'B') {
//                         selected_piece_id_p2 = id;
//                         std::cout << "[DEBUG] Found black piece: " << id << std::endl;
//                         break;
//                     }
//                 }
//                 if (!selected_piece_id_p2.empty()) {
//                     selecting_source_p2 = false;
//                     std::cout << "[DEBUG] Source selected, now select destination" << std::endl;
//                 } else {
//                     std::cout << "[DEBUG] No black piece found" << std::endl;
//                 }
//             } else {
//                 dst_p2 = selected_cell_p2;
//                 std::cout << "[DEBUG] Creating move command from (" << src_p2.row << "," << src_p2.col << ") to (" << dst_p2.row << "," << dst_p2.col << ")" << std::endl;
//                 Command cmd(game_time_ms(), selected_piece_id_p2, "move", src_p2, dst_p2);
//                 user_input_queue.push(cmd);
//                 selecting_source_p2 = true;
//                 std::cout << "[DEBUG] Command queued for Player 2" << std::endl;
//             }
//         }

//         _resolve_collisions();
//     }
//     if(websocket_callback_) {
//         websocket_callback_("game_won", "Player 1");
//     }
//     publisher.publish("game_won", "");
//     _announce_win();
//     cv::destroyAllWindows();
// }

void Game::_process_input(const Command& cmd, const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece) {
    auto cell2_to_piece = build_cell_to_piece_map();
    if (pieces_.find(cmd.piece_id) != pieces_.end()) {
        pieces_[cmd.piece_id]->on_command(cmd, game_time_ms(),cell2_to_piece, command_success_);
        // if(command_success_)
        //     publisher.publish("move_started", cmd.piece_id);
        // else
        //     publisher.publish("failed", cmd.piece_id);
        if(command_success_) {
            std::string move_info = cmd.piece_id + " " + std::to_string(cmd.src.row) + "," + std::to_string(cmd.src.col) + "->" + std::to_string(cmd.dst.row) + "," + std::to_string(cmd.dst.col);
            if(websocket_callback_) {
                websocket_callback_("move_started", move_info);
            }
            publisher.publish("move_started", move_info);
        }
        else
        {
            if(websocket_callback_) {
                websocket_callback_("move_failed", cmd.piece_id);
            }
            publisher.publish("failed", cmd.piece_id);
        }
    }
}

// void Game::_draw() {
//     if (!board_.img || board_.img->empty()) return;
//     cv::Mat& current_mat = board_.img->get_mat();
//     // יצירת רקע עם תמונת רקע אמיתית
//     cv::Mat background = cv::imread("C:\\CTD25\\cpp\\background.jpg");
//     if (!background.empty()) {
//         cv::resize(background, background, cv::Size(current_mat.cols, current_mat.rows));
//         background.copyTo(current_mat);

//     } else {
//         current_mat.setTo(cv::Scalar(50, 50, 50));
//     }
    
//     // טען לוח בגודל קטן יותר
//     cv::Mat board_only = cv::imread("C:\\CTD25\\cpp\\board.png");

//     if (!board_only.empty()) {

//         cv::resize(board_only, board_only, cv::Size(700, 700));
//         int offset_x = (current_mat.cols - 700) / 2;
//         int offset_y = (current_mat.rows - 700) / 2;
        
//         if (offset_x >= 0 && offset_y >= 0) {
//             cv::Rect roi(offset_x, offset_y, 700, 700);
//             board_only.copyTo(current_mat(roi));

//         }
        
//         // עדכון האופסט בלוח
//         board_.offset_x = offset_x;
//         board_.offset_y = offset_y;

//     }
    
//     // עדכון גודל התא
//     board_.cell_H_pix = 87;
//     board_.cell_W_pix = 87;
    
//     // ציור החיילים
//     for (const auto& [id, piece] : pieces_) {
//         if (!piece) continue;

//         piece->draw_on_board(board_, 0);

//     }
    
//     // ציור ריבוע הבחירה
//     int offset_x = (current_mat.cols - 700) / 2;
//     int offset_y = (current_mat.rows - 700) / 2;
//     int cell_size = 87;

//     // סמן שחקן 1 (לבן)
//     int x1 = offset_x + selected_cell_p1.col * cell_size;
//     int y1 = offset_y + selected_cell_p1.row * cell_size;
//     cv::rectangle(current_mat, cv::Rect(x1, y1, cell_size, cell_size), cv::Scalar(0, 255, 0), 3);
    
//     // סמן שחקן 2 (שחור)
//     int x2 = offset_x + selected_cell_p2.col * cell_size;
//     int y2 = offset_y + selected_cell_p2.row * cell_size;
//     cv::rectangle(current_mat, cv::Rect(x2, y2, cell_size, cell_size), cv::Scalar(0, 0, 255), 3);


    
//     cv::Scalar color = command_success_ ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);


//     // הוספת הצגת ניקוד ומהלכים
//     _draw_game_info(current_mat);
// }

// void Game::_draw_game_info(cv::Mat& mat) {
//     auto scores = score_board.get_scores();
//     auto recent_moves = move_list.get_last_moves(20);
    
//     // מידע שחקן שחור (שמאל)
//     int left_x = 10;
//     int left_y = 30;
    
//     cv::putText(mat, "PLAYER 2 (BLACK):", cv::Point(left_x, left_y), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
//     left_y += 30;
    
//     // ניקוד שחקן שחור
//     int black_score_val = (scores.find("Black") != scores.end()) ? scores.at("Black") : 0;
//     std::string score_text = "Score: " + std::to_string(black_score_val);
//     cv::putText(mat, score_text, cv::Point(left_x, left_y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
//     left_y += 40;
    
//     // מהלכים של שחקן שחור
//     cv::putText(mat, "MOVES:", cv::Point(left_x, left_y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
//     left_y += 25;
    
//     int black_move_count = 0;
//     for (const auto& move : recent_moves) {
//         if (move.find("B") != std::string::npos && black_move_count < 15) {
//             cv::putText(mat, move, cv::Point(left_x, left_y), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0), 1);
//             left_y += 18;
//             black_move_count++;
//         }
//         if (left_y > mat.rows - 50) break;
//     }
    
//     // מידע שחקן לבן (ימין)
//     int right_x = mat.cols - 250;
//     int right_y = 30;
    
//     cv::putText(mat, "PLAYER 1 (WHITE):", cv::Point(right_x, right_y), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
//     right_y += 30;
    
//     // ניקוד שחקן לבן
//     int white_score_val = (scores.find("White") != scores.end()) ? scores.at("White") : 0;
//     score_text = "Score: " + std::to_string(white_score_val);
//     cv::putText(mat, score_text, cv::Point(right_x, right_y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
//     right_y += 40;
    
//     // מהלכים של שחקן לבן
//     cv::putText(mat, "MOVES:", cv::Point(right_x, right_y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
//     right_y += 25;
    
//     int white_move_count = 0;
//     for (const auto& move : recent_moves) {
//         if (move.find("W") != std::string::npos && white_move_count < 15) {
//             cv::putText(mat, move, cv::Point(right_x, right_y), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0), 1);
//             right_y += 18;
//             white_move_count++;
//         }
//         if (right_y > mat.rows - 50) break;
//     }
//     game_messages.draw_message(mat);

// }


bool Game::_is_win() const {
    return false; // To be implemented
}



void Game::_announce_win() {
    // To be implemented
}

std::vector<std::shared_ptr<Piece>> Game::load_pieces_from_file(const std::string& filename, PieceFactory& factory) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return {};
    }
    json data;
    file >> data;
    std::cout << "Data from file:\n" << data.dump(4) << std::endl;

    std::vector<std::shared_ptr<Piece>> pieces;
    for (const auto& item : data) {
        std::string piece_type = item["id"];
        int row = item["position"][0];
        int col = item["position"][1];
        std::string unique_id = piece_type + "_pos_" + std::to_string(row) + "_" + std::to_string(col);
        auto piece = factory.create_piece(piece_type, {row, col});
        piece->set_id(unique_id);
        pieces.push_back(piece);
    }
    return pieces;
}


void Game::_resolve_collisions() {
    // מיפוי בין תאים לרשימת חתיכות שנמצאות באותו תא
    std::unordered_map<Cell, std::vector<std::shared_ptr<Piece>>, Cell::Hash> cell_to_pieces;

    for (const auto& [id, piece] : pieces_) {
        cell_to_pieces[piece->get_cell()].push_back(piece);
    }
    auto cell2_to_pieces = build_cell_to_piece_map();
    for (const auto& [cell, plist] : cell_to_pieces) {
        if (plist.size() < 2) continue;

        // מוצאים את החתיכה שהגיעה אחרונה (לפי start_ms)
        auto winner = *std::max_element(plist.begin(), plist.end(), [](const auto& a, const auto& b) {
            return a->get_state()->get_physics()->get_start_time_ms() < b->get_state()->get_physics()->get_start_time_ms();
        });

        for (const auto& piece : plist) {
            if (piece == winner) continue;

            if (piece->get_state()->get_physics()->can_be_captured()) {
                std::cout << "[Collision] Piece " << piece->get_id() << " captured at ("
                          << cell.row << "," << cell.col << ")\n";
                pieces_.erase(piece->get_id());
                if(websocket_callback_) {
                    websocket_callback_("piece_captured", piece->get_id());
                }
                publisher.publish("piece_captured", piece->get_id());
            }
        }
    }
} 


std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash> Game::build_cell_to_piece_map() const {
    std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash> map;
    for (const auto& [id, piece] : pieces_) {
        map[piece->get_cell()] = piece;
    }
    return map;
}

void Game::set_websocket_callback(std::function<void(const std::string&, const std::string&)> callback) {
    websocket_callback_ = callback;
}

void Game::add_command(const Command& cmd) {
    std::lock_guard<std::mutex> lock(command_queue_mutex);
    user_input_queue.push(cmd);
}
