#include "game.hpp"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

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

void Game::_process_input(const Command& cmd, const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece) {
    auto cell2_to_piece = build_cell_to_piece_map();
    if (pieces_.find(cmd.piece_id) != pieces_.end()) {
        pieces_[cmd.piece_id]->on_command(cmd, game_time_ms(),cell2_to_piece, command_success_);
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