#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <opencv2/opencv.hpp>

class Background {
public:
    Background(const std::string& background_path,
               int window_width, int window_height);

    void render_board(const cv::Mat& board_img);
    void show(const std::string& window_name);

private:
    cv::Mat background_;
    int win_w_, win_h_;
};

#endif // BACKGROUND_HPP
