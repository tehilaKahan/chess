#include "img.hpp"
#include <iostream>
#include <stdexcept>

Img::Img() {
    // Constructor - img is automatically initialized as empty
}

Img& Img::read(const std::string& path,
               const std::pair<int, int>& size,
               bool keep_aspect,
               int interpolation) {
    img = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (img.empty()) {
        throw std::runtime_error("Cannot load image: " + path);
    }

    if (size.first != 0 && size.second != 0) {  // Check if size is not empty
        int target_w = size.first;
        int target_h = size.second;
        int h = img.rows;
        int w = img.cols;

        if (keep_aspect) {
            double scale = std::min(static_cast<double>(target_w) / w, 
                                   static_cast<double>(target_h) / h);
            int new_w = static_cast<int>(w * scale);
            int new_h = static_cast<int>(h * scale);
            cv::resize(img, img, cv::Size(new_w, new_h), 0, 0, interpolation);
        } else {
            cv::resize(img, img, cv::Size(target_w, target_h), 0, 0, interpolation);
        }
    }

    return *this;
}


void Img::draw_on(Img& other_img, int x, int y) {
    if (img.empty() || other_img.img.empty()) {
        throw std::runtime_error("Both images must be loaded before drawing.");
    }

    cv::Mat source_img = img;
    cv::Mat& target_img = other_img.img;

    // התאמת פורמט בין BGR ל־BGRA
    if (source_img.channels() != target_img.channels()) {
        if (source_img.channels() == 3 && target_img.channels() == 4) {
            cv::cvtColor(source_img, source_img, cv::COLOR_BGR2BGRA);
        }
        else if (source_img.channels() == 4 && target_img.channels() == 3) {
            cv::cvtColor(source_img, source_img, cv::COLOR_BGRA2BGR);
        }
    }
    int h = source_img.rows;
    int w = source_img.cols;
    int H = target_img.rows;
    int W = target_img.cols;

    if (x < 0 || y < 0 || x + w > W || y + h > H) {
        throw std::runtime_error("Image does not fit at the specified position.");
    }

    cv::Mat roi = target_img(cv::Rect(x, y, w, h));

    if (source_img.channels() == 4) {
        // פיצול ערוצים
        std::vector<cv::Mat> src_channels;
        cv::split(source_img, src_channels); // B, G, R, A

        cv::Mat alpha;
        src_channels[3].convertTo(alpha, CV_32FC1, 1.0 / 255.0); // normalize alpha

        // הכנת ה־ROI לערוצים
        std::vector<cv::Mat> roi_channels;
        cv::split(roi, roi_channels); // B, G, R, (possibly A)

        for (int c = 0; c < 3; ++c) {
            src_channels[c].convertTo(src_channels[c], CV_32FC1);
            roi_channels[c].convertTo(roi_channels[c], CV_32FC1);

            // alpha blending: out = src * alpha + dst * (1 - alpha)
            roi_channels[c] = src_channels[c].mul(alpha) + roi_channels[c].mul(1.0 - alpha);
            roi_channels[c].convertTo(roi_channels[c], CV_8UC1);
        }

        cv::merge(roi_channels, roi);


    }
    else {
    // יצירת מסכה לרקע לבן
    cv::Mat mask;
    cv::inRange(source_img, cv::Scalar(240, 240, 240), cv::Scalar(255, 255, 255), mask);
    cv::bitwise_not(mask, mask);
    
    // העתקה עם מסכה
    source_img.copyTo(roi, mask);
}



}


void Img::put_text(const std::string& txt, int x, int y, double font_size,
                   const cv::Scalar& color, int thickness) {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    
    cv::putText(img, txt, cv::Point(x, y),
                cv::FONT_HERSHEY_SIMPLEX, font_size,
                color, thickness, cv::LINE_AA);
}

void Img::show() {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    
    cv::imshow("Image", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
} 

void Img::create_blank(int width, int height, bool with_alpha) {
    int type = with_alpha ? CV_8UC4 : CV_8UC3;
    img = cv::Mat(height, width, type, cv::Scalar(0, 0, 0, 0)); // רקע שקוף
}

int Img::width() const {
    return img.cols;
}

int Img::height() const {
    return img.rows;
}
