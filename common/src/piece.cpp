#include "piece.hpp"
#include <iostream>
#include "state.hpp"
#include "board.hpp"
#include "mockImg.hpp"
Piece::Piece(const std::string& piece_id, const std::shared_ptr<State>&  init_state)
    : piece_id_(piece_id), state_(init_state->clone())
{
}
bool Piece::is_command_possible(const Command& cmd,const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece) const {
    return state_->is_command_possible(cmd, cell_to_piece);
}

void Piece::on_command(const Command& cmd, int now_ms,const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece, bool& command_success) {
    std::cout << "Checking if command is possible: " << is_command_possible(cmd,cell_to_piece) << std::endl;
    if (is_command_possible(cmd,cell_to_piece)) {
        std::cout << "Command accepted: " << cmd.to_string() << std::endl;
        state_ = state_->process_command(cmd, now_ms);
        state_ = state_->update(now_ms);
    } else {
        command_success = false; 
        std::cout << "Command rejected: " << cmd.to_string() << std::endl;
    }
}


Cell Piece::get_cell() const {
        return state_->get_current_cell();
}
Cell Piece::get_position() const {
    return state_->get_physics()->get_pos();
}
void Piece::reset(int start_ms) {
Command idle_cmd(start_ms, piece_id_, "None",get_cell(), get_cell());

state_->reset(idle_cmd); 
}

void Piece::update(int now_ms) {
    auto new_state = state_->update(now_ms);
    if (new_state != nullptr) {
        state_ = new_state;
    }
}

void Piece::draw_on_board(Board& board, int now_ms) {
    // 1. מיקום
    Cell cell = this->get_cell();

    //Cell cell = state_->get_current_cell();
    //auto [x, y] = board.cell_to_pixels(cell);
    // auto [x, y] = cell;
    auto [x, y] = board.cell_to_pixels(cell);


    // 2. תמונת המסגרת הנוכחית
    auto graphics = state_->get_graphics();
    Img& img_frame = graphics->get_img(); 
    //std::cout << "Drawing piece at cell: " << cell.row << ", " << cell.col << std::endl;    
    // 3. ציור על תמונת הלוח

        // שנה גודל החייל ל-87x87
    cv::Mat piece_mat = img_frame.get_mat();
    cv::Mat resized_piece;
    cv::resize(piece_mat, resized_piece, cv::Size(87, 87));
    
    Img temp_img;
    temp_img.get_mat() = resized_piece;
    temp_img.draw_on(*board.img, x, y);
}


//     img_frame.draw_on(*board.img, x, y);
//     //std::cout << "[Draw] Drawing piece at: (" << x << ", " << y << ")\n";

//     // 4. (אופציונלי) ציור אפקטים נוספים (לדוגמה cooldown)
//     // if (state_->is_in_cooldown()) {
//     //     board.img.draw_overlay_cooldown(x, y, ...);
//     // }
// }
