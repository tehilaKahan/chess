#ifndef PIECE_HPP
#define PIECE_HPP

#include <string>
#include "command.hpp"
#include "cell.hpp"

#include <memory>
class Board;
class State; 
class Piece {
public:
    Piece(const std::string& piece_id, const std::shared_ptr<State>& init_state);
    Cell get_position() const ;

    void on_command(const Command& cmd, int now_ms,const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece, bool& command_success);

    void reset(int start_ms);
    bool Piece::is_command_possible(const Command& cmd,const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece) const ;

    void update(int now_ms);
    std::shared_ptr<State> get_state() const {
        return state_;
    }
    void draw_on_board(Board& board, int now_ms);
     Cell get_cell() const ;

    std::string get_id() const { return piece_id_; }
    void set_id(const std::string& new_id){piece_id_ = new_id;}

private:
    std::string piece_id_;
    std::shared_ptr<State> state_;
};

#endif // PIECE_HPP
