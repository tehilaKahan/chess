#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include "img.hpp"
#include "../../common/src/command.hpp"

class Graphics {
public:
    Graphics(const std::filesystem::path& sprites_folder,
             std::pair<int, int> cell_size,
             bool loop = true,
             float fps = 6.0f);

    std::shared_ptr<Graphics> copy() const;
    void reset(const Command& cmd);
    void update(int now_ms);
    Img& Graphics::get_img() ;

private:
    std::filesystem::path sprites_folder_;
    std::pair<int, int> cell_size_;
    bool loop_;
    float fps_;

    std::vector<Img> frames_;
    int current_frame_;
    int frame_time_ms_;
    int last_update_time_ms_;
};

#endif // GRAPHICS_H
