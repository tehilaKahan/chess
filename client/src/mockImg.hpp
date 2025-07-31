#ifndef MOCKIMG_HPP
#define MOCKIMG_HPP

#include <vector>
#include <string>
#include <utility>
#include "img.hpp"
#include <memory>

class MockImg : public Img {
public:
    MockImg();

    MockImg& read(const std::string& path);


    void draw_on(Img& other_img, int x, int y) override;
    void put_text(const std::string& txt, int x, int y, double font_size);

    void show();

    static void reset();

    // סטטיות לשמירת היסטוריית הקריאות
    static std::vector<std::pair<int,int>> traj;              // מיקומי draw_on
    static std::vector<std::pair<std::pair<int,int>, std::string>> txt_traj; // מיקומי put_text עם טקסט
};

#endif // MOCKIMG_HPP
