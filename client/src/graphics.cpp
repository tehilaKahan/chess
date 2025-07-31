#include "graphics.hpp"
#include <chrono>

#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>

Graphics::Graphics(const std::filesystem::path& sprites_folder,
                   std::pair<int, int> cell_size,
                   bool loop,
                   float fps)
    : sprites_folder_(sprites_folder),
      cell_size_(cell_size),
      loop_(loop),
      fps_(fps),
      current_frame_(0),
      frame_time_ms_(static_cast<int>(1000.0f / fps)),
      last_update_time_ms_(0)
{


    int i = 1;
    while (true) {

        //std::filesystem::path img_path = sprites_folder_ / "sprites" / (std::to_string(i) + ".png");
        std::filesystem::path img_path = (sprites_folder_ / (std::to_string(i) + ".png")).make_preferred();

        if (!std::filesystem::exists(img_path)) break;

        cv::Mat mat = cv::imread(img_path.string());
        if (mat.empty()) {
            std::cerr << "Warning: failed to load " << img_path << "\n";
            break;
        }

        // שינוי גודל לתא אם צריך
        cv::resize(mat, mat, cv::Size(cell_size_.first, cell_size_.second));

        Img img;
        img.get_mat() = mat;
        frames_.push_back(img); 
        ++i;
    }

    if (frames_.empty()) {
        std::cerr << "Warning: no frames loaded for " << sprites_folder << "\n";
    }
}






void Graphics::reset(const Command& cmd) {
    current_frame_ = 0;
    last_update_time_ms_ = cmd.timestamp ;

}





void Graphics::update(int now_ms) {
    if (frames_.empty()) return;  // אם אין פריימים, לא עושים כלום

    int ms_per_frame = static_cast<int>(1000 / fps_);  // כמה מילישניות כל פריים

    int elapsed = now_ms - last_update_time_ms_;
    if (elapsed >= ms_per_frame) {
        // כמה פריימים צריך לקפוץ קדימה
        int frames_to_advance = elapsed / ms_per_frame;

        current_frame_ += frames_to_advance;

        if (loop_) {
            current_frame_ %= frames_.size();  // חזרה להתחלה בלולאה
        } else if (current_frame_ >= frames_.size()) {
            current_frame_ = static_cast<int>(frames_.size()) - 1;  // נשאר בפריים האחרון
        }

        last_update_time_ms_ = now_ms;  // עדכון זמן העדכון האחרון
    }
}

std::shared_ptr<Graphics> Graphics::copy() const {
    return std::make_shared<Graphics>(sprites_folder_, cell_size_, loop_, fps_);
}

Img& Graphics::get_img() {
    if (frames_.empty())
        throw std::runtime_error("No frames available");
    return frames_[current_frame_];
} 

