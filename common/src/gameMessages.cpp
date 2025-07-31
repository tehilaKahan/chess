#include "gameMessages.hpp"

void GameMessages::on_event(const std::string& event_name, const std::string& data) {
    if (event_name == "game_started") {
        current_message = "GAME STARTED - GOOD LUCK!";
        show_message = true;
        message_start_time = std::chrono::steady_clock::now();
    } else if (event_name == "game_won") {
        current_message = "GAME OVER - WELL PLAYED!";
        show_message = true;
        message_start_time = std::chrono::steady_clock::now();
    }
}

void GameMessages::draw_message(cv::Mat& mat) {
    if (!show_message) return;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - message_start_time).count();
    
    if (elapsed > display_duration_ms) {
        show_message = false;
        return;
    }
    
    int x = mat.cols / 2 - 200;
    int y = mat.rows / 2;
    cv::putText(mat, current_message, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 255, 255), 3);
}
