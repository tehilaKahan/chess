#ifndef MOVELIST_HPP
#define MOVELIST_HPP

#include "ISubscriber.hpp"
#include <vector>
#include <string>

class MoveList : public ISubscriber {
private:
    std::vector<std::string> moves;

public:

    // ~MoveList() override = default; // לא חובה, אבל אפשר
    void on_event(const std::string& event_name, const std::string& data) override;

    std::vector<std::string> get_last_moves(int count = 20) const {
    if (moves.size() <= count) return moves;
    return std::vector<std::string>(moves.end() - count, moves.end());
    }


    // שליפה לרכיב אחר (GUI למשל)
    const std::vector<std::string>& get_moves() const { return moves; }
};

#endif // MOVELIST_HPP
