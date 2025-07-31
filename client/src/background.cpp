#include "background.hpp"

Background::Background(const std::string& background_path,
                       int window_width, int window_height)
    : win_w_(window_width), win_h_(window_height)
{
    background_ = cv::imread(background_path, cv::IMREAD_UNCHANGED);
    if (background_.empty()) {
        background_ = cv::Mat::zeros(win_h_, win_w_, CV_8UC3); // רקע שחור
    } else {
        cv::resize(background_, background_, cv::Size(win_w_, win_h_));
    }
}

void Background::render_board(const cv::Mat& board_img) {
    int x = (win_w_ - board_img.cols) / 2;
    int y = (win_h_ - board_img.rows) / 2;

    cv::Rect roi(cv::Point(x, y), board_img.size());

    // אם יש שקיפות – נשלב עם רקע
    if (board_img.channels() == 4) {
        for (int row = 0; row < board_img.rows; ++row) {
            for (int col = 0; col < board_img.cols; ++col) {
                cv::Vec4b fg = board_img.at<cv::Vec4b>(row, col);
                cv::Vec3b& bg = background_.at<cv::Vec3b>(y + row, x + col);
                float alpha = fg[3] / 255.0f;
                for (int c = 0; c < 3; ++c)
                    bg[c] = bg[c] * (1 - alpha) + fg[c] * alpha;
            }
        }
    } else {
        board_img.copyTo(background_(roi));
    }
}

void Background::show(const std::string& window_name) {
    cv::imshow(window_name, background_);
}
