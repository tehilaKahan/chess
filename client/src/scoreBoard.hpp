#ifndef SCOREBOARD_HPP
#define SCOREBOARD_HPP

#include "ISubscriber.hpp"
#include <string>
#include <map>

class ScoreBoard : public ISubscriber {
public:
    void on_event(const std::string& event_name, const std::string& data) override;
    std::string get_capturing_player(const std::string& piece_id) const;
    int get_piece_value(const std::string& piece_id) const;

    void display_scores() const;
    const std::map<std::string, int>& get_scores() const { return player_scores_; }


private:
    std::map<std::string, int> player_scores_;
};

#endif // SCOREBOARD_HPP
