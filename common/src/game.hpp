#pragma once

#include <unordered_map>
#include <memory>
#include <queue>
#include <chrono>
#include "board.hpp"
#include "command.hpp"
#include "piece.hpp"
#include "cell.hpp"
#include "pieceFactory.hpp"
#include "../../external/nlohmann/json.hpp"
#include <fstream>     
#include "publisher.hpp"
#include "gameMessages.hpp"
#include <functional>
#include <mutex>

class Game {
public:
    Game(const std::vector<std::shared_ptr<Piece>>& pieces, const Board& board);
    std::vector<std::shared_ptr<Piece>>  load_pieces_from_file(const std::string& filename, PieceFactory& factory) ;
    std::chrono::steady_clock::time_point start_tp;
    std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>  build_cell_to_piece_map() const ;

    void run();
    void set_websocket_callback(std::function<void(const std::string&, const std::string&)> callback);
    void add_command(const Command& cmd);
    int game_time_ms() const;

private:
    bool command_success_=true;
    std::unordered_map<std::string, std::shared_ptr<Piece>> pieces_;
    Board board_;
    std::queue<Command> user_input_queue;
    Cell selected_cell {0,0};
    Board clone_board() const;
    void start_user_input_thread();
    std::mutex command_queue_mutex;
    bool running_ = false;

    Publisher publisher;
        
    std::function<void(const std::string&, const std::string&)> websocket_callback_;
    
    void _process_input(const Command& cmd,const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece);
    bool _show();
    bool _is_win() const;
    void _announce_win();
    void _resolve_collisions() ;
};