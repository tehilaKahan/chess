#ifndef GAMEMESSAGES_HPP
#define GAMEMESSAGES_HPP

#include "ISubscriber.hpp"
#include <string>
#include <chrono>
#include <opencv2/opencv.hpp>


class GameMessages : public ISubscriber {
private:
    std::string current_message;
    std::chrono::steady_clock::time_point message_start_time;
    int display_duration_ms = 3000;
    bool show_message = false;

public:
    void on_event(const std::string& event_name, const std::string& data) override;
    void draw_message(cv::Mat& mat);
};

#endif
