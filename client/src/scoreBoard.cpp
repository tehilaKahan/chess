// #include "scoreboard.hpp"
// #include <iostream>
// #include <sstream>

// void ScoreBoard::on_event(const std::string& event_name, const std::string& data) {
//     if (event_name == "piece_captured") {
//         std::istringstream iss(data);
//         std::string player;
//         int points;
//         if (iss >> player >> points) {
//             player_scores_[player] += points;
//         }
//     }
// }

// void ScoreBoard::display_scores() const {
//     std::cout << "Current Scores:\n";
//     for (const auto& pair : player_scores_) {
//         std::cout << pair.first << ": " << pair.second << " points\n";
//     }
// }

// עדכון scoreBoard.cpp
#include "scoreboard.hpp"
#include <iostream>
#include <map>

void ScoreBoard::on_event(const std::string& event_name, const std::string& data) {
    if (event_name == "piece_captured") {
        int points = get_piece_value(data);
        std::string capturing_player = get_capturing_player(data);
        player_scores_[capturing_player] += points;
    }
}

int ScoreBoard::get_piece_value(const std::string& piece_id) const {
    if (piece_id.find("P") != std::string::npos) return 1;
    if (piece_id.find("N") != std::string::npos) return 3;
    if (piece_id.find("B") != std::string::npos) return 3;
    if (piece_id.find("R") != std::string::npos) return 5;
    if (piece_id.find("Q") != std::string::npos) return 9;
    return 0;
}

std::string ScoreBoard::get_capturing_player(const std::string& piece_id) const {
    if (piece_id.find("W") != std::string::npos) return "Black";
    if (piece_id.find("B") != std::string::npos) return "White";
    return "Unknown";
}

