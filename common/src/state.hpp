#ifndef STATE_HPP
#define STATE_HPP

#include <memory>
#include <string>
#include <map>
#include "command.hpp"
#include "moves.hpp"
#include "graphics.hpp"
#include "physics.hpp"
#include "cell.hpp"

class State : public std::enable_shared_from_this<State> {
public:
    std::string name; 
    State() = default; 
    State(std::shared_ptr<Moves> moves,
          std::shared_ptr<Graphics> graphics,
          std::shared_ptr<Physics> physics);

    void set_transition(const std::string& event, std::shared_ptr<State> target);


    Cell get_current_cell() const {
        return current_command_.dst; 
    }

        std::shared_ptr<Graphics> get_graphics() const {
        return graphics_;
    }
        std::shared_ptr<Physics> get_physics() const {
        return physics_;
    }
    void reset(const Command& cmd);
    bool State::is_command_possible(const Command& cmd,const std::unordered_map<Cell, std::shared_ptr<Piece>, Cell::Hash>& cell_to_piece) const ;

    //bool can_transition(int now_ms) const;

    std::shared_ptr<State> process_command(const Command& cmd, int now_ms) ;

    std::shared_ptr<State> update(int now_ms);
    std::shared_ptr<State> State::clone() const;

    Command get_command() const;
    
    void set_next_state_when_finished(std::shared_ptr<State> state) {
        next_state_when_finished_ = state;
    }

    std::shared_ptr<State> get_next_state_when_finished() const {
        return next_state_when_finished_;
    }

private:
    std::shared_ptr<Moves> moves_;
    std::shared_ptr<Graphics> graphics_;
    std::shared_ptr<Physics> physics_;
    std::shared_ptr<State> next_state_when_finished_; // מצביע למצב הבא בסיום פעולה

    Command current_command_;

    std::map<std::string, std::shared_ptr<State>> transitions_;

    // משתנים פנימיים למעקב אחרי זמן, מצב פנימי וכו' ניתן להוסיף כאן
};

#endif // STATE_HPP
