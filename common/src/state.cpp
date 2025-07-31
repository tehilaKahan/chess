#include "state.hpp"
#include <iostream>

State::State(std::shared_ptr<Moves> moves,
             std::shared_ptr<Graphics> graphics,
             std::shared_ptr<Physics> physics)
    : moves_(std::move(moves)), graphics_(std::move(graphics)), physics_(std::move(physics))
{
}

void State::set_transition(const std::string& event, std::shared_ptr<State> target) {
    transitions_[event] = target;
}

// void State::reset(const Command& cmd) {
//     current_command_ = cmd;
//     if (graphics_) graphics_->reset(cmd);
//     if (physics_) physics_->reset(cmd);
// }
void State::reset(const Command& cmd) {
    current_command_ = cmd;
    if (graphics_) graphics_->reset(cmd);
    if (physics_) {
        physics_->reset(cmd);
    }

}


// bool State::can_transition(int now_ms) const {
//     if (physics_) {
//         // פשוט בודקים אם הפיזיקה סיימה את הפעולה
//         return physics_->is_finished();
//     }
//     return true; // אם אין פיזיקה, תמיד ניתן לעבור
// }



std::shared_ptr<State> State::process_command(const Command& cmd, int now_ms)  {
    auto it = transitions_.find(cmd.type);
    std::cout << "[State] Processing command of type: " << cmd.type << std::endl;
    std::cout << "[State] Available transitions:" << std::endl;
    for (const auto& [event, state] : transitions_) {
        std::cout << "- " << event << std::endl;
    }

    if (it != transitions_.end()) {
        auto next_state = it->second->clone();
        next_state->reset(cmd);
        return next_state;
    }
    // אם אין מעבר מתאים, תחזיר את המצב הנוכחי
    return std::const_pointer_cast<State>(shared_from_this());
}

// bool State::is_command_possible(const Command& cmd) const {
//     return transitions_.find(cmd.type) != transitions_.end();
// }
bool State::is_command_possible(const Command& cmd,const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece) const {
    return moves_->is_valid(cmd ,cell_to_piece );
}


std::shared_ptr<State> State::clone() const {
    // פשוט יוצרים עותק חדש עם copy constructors של כל הרכיבים
    auto moves_clone = moves_ ? std::make_shared<Moves>(*moves_) : nullptr;
    auto graphics_clone = graphics_ ? graphics_->copy() : nullptr; // כי Graphics כבר יש לה copy()
    //auto physics_clone = physics_ ? std::make_shared<Physics>(*physics_) : nullptr;
    auto physics_clone = physics_ ? physics_->clone() : nullptr;

    auto new_state = std::make_shared<State>(moves_clone, graphics_clone, physics_clone);

    // מעתיקים את המעברים (הפניות לאותם אובייקטים, לא מעתיקים אותם)
    new_state->transitions_ = transitions_;

    // מעתיקים את הפקודה הנוכחית
    new_state->current_command_ = current_command_;

        // **זה החסר - מעתיקים את next_state_when_finished_**
    new_state->next_state_when_finished_ = next_state_when_finished_;


    return new_state;
}





// std::shared_ptr<State> State::update(int now_ms) {
//     if (graphics_) graphics_->update(now_ms);

//     Command cmd = physics_ ? physics_->update(now_ms) : Command();

//     if (!cmd.is_empty()) {
//         return process_command(cmd, now_ms);
//     }

//     // אם אין פקודה חדשה, אבל הפיזיקה סיימה, נעבור אוטומטית למצב הבא
//     if (can_transition(now_ms)) {
//         if (next_state_when_finished_) {
//             next_state_when_finished_->reset(current_command_);
//             return next_state_when_finished_;
//         }
//     }

//     return shared_from_this();
// }

// std::shared_ptr<State> State::update(int now_ms) {
//     if (graphics_) graphics_->update(now_ms);

//     Command cmd = physics_ ? physics_->update(now_ms) : Command();

//     if (!cmd.is_empty()) {
//         return process_command(cmd, now_ms);
//     }
        
//     return shared_from_this();
// }

// std::shared_ptr<State> State::update(int now_ms) {
//     if (graphics_) graphics_->update(now_ms);

//     Command cmd = physics_ ? physics_->update(now_ms) : Command();

//     if (!cmd.is_empty()) {
//         return process_command(cmd, now_ms);
//     }

//     // אם אין פקודה חדשה, אבל הפיזיקה סיימה, נעבור אוטומטית למצב הבא
//     if (physics_ && physics_->is_finished()) {
//         if (next_state_when_finished_) {
//             auto next_state = next_state_when_finished_->clone();
//             next_state->reset(current_command_);
//             return next_state;
//         }
//     }

//     return shared_from_this();
// }

std::shared_ptr<State> State::update(int now_ms) {
    if (graphics_) graphics_->update(now_ms);

    Command cmd = physics_ ? physics_->update(now_ms) : Command();

    if (!cmd.is_empty()) {
        return process_command(cmd, now_ms);
    }

    // אם אין פקודה חדשה, אבל הפיזיקה סיימה, נעבור אוטומטית למצב הבא
    if (physics_) {
        bool is_finished = physics_->is_finished();
        
        if (is_finished) {
            if (next_state_when_finished_) {
                auto next_state = next_state_when_finished_->clone();
                next_state->reset(current_command_);
                return next_state;
            } 
        }
    }

    return shared_from_this();
}


Command State::get_command() const {
    return current_command_;
}
