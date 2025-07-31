#include "mockImg.hpp"
#include <iostream>

// אתחול הווקטורים הסטטיים מחוץ למחלקה
std::vector<std::pair<int,int>> MockImg::traj = {};
std::vector<std::pair<std::pair<int,int>, std::string>> MockImg::txt_traj = {};

MockImg::MockImg() : Img() {
    this->img = cv::Mat::zeros(512, 512, CV_8UC3);
}

MockImg& MockImg::read(const std::string& path) {
    std::cout << "Mock read called with: " << path << std::endl;
    return *this;
}

void MockImg::draw_on(Img& other_img, int x, int y) {
    traj.emplace_back(x, y);
}

void MockImg::put_text(const std::string& txt, int x, int y, double /*font_size*/) {
    txt_traj.emplace_back(std::make_pair(x,y), txt);
}

void MockImg::show() {
    std::cout << "Mock show called" << std::endl;
}

void MockImg::reset() {
    traj.clear();
    txt_traj.clear();
}
