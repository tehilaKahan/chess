#pragma once

#include <unordered_map>
#include <memory>
#include <queue>
#include <chrono>
#include "../../common/src/board.hpp"
#include "../../common/src/command.hpp"
#include "../../common/src/piece.hpp"
#include "../../common/src/cell.hpp"
#include "../../common/src/pieceFactory.hpp"
#include "json.hpp"
#include <fstream>     
#include "../../common/src/publisher.hpp"
#include "../../common/src/gameMessages.hpp"
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

private:
    bool command_success_=true;
    std::unordered_map<std::string, std::shared_ptr<Piece>> pieces_;
    Board board_;
    std::queue<Command> user_input_queue;
    Cell selected_cell {0,0};
    int game_time_ms() const;
    Board clone_board() const;
    void start_user_input_thread();
    std::mutex command_queue_mutex;
    bool running_ = false;  // הוסף שורה זו

    Publisher publisher;
        
    std::function<void(const std::string&, const std::string&)> websocket_callback_;



        // משתנים עבור שני שחקנים
    // Cell selected_cell_p1 {0,0};
    // Cell selected_cell_p2 {7,7};
    // std::string selected_piece_id_p1;
    // std::string selected_piece_id_p2;
    // bool selecting_source_p1 = true;
    // bool selecting_source_p2 = true;
    // Cell src_p1, dst_p1;
    // Cell src_p2, dst_p2;



    
    void _process_input(const Command& cmd,const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece);
    // void _draw();
    bool _show();
    bool _is_win() const;
    void _announce_win();
    void _resolve_collisions() ;
    // void _draw_game_info(cv::Mat& mat);

};

