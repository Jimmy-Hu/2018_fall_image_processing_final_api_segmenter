#ifndef FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_CV_BRIDGE_HPP_INCLUDED
#define FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_CV_BRIDGE_HPP_INCLUDED

#include "core.hpp"
#include <opencv/cv.h>

namespace api {
namespace segmenter {

std_image_type cv_mat_to_std_image(cv::Mat in) {
    cv::Mat in_;
    std_image_type out;
    auto & size = std::get<0>(out);
    auto & data = std::get<1>(out);
    int xn;
    int yn;
    int cn;
    xn = in.cols;
    yn = in.rows;
    cn = in.channels();
    size.first = xn;
    size.second = yn;
    data.resize(in.rows * in.cols);
    for (int x = 0; x < xn; ++x) {
        for (int y = 0; y < yn; ++y) {
            // NOTE:
            // OpenCV use BGR format as default, our format is RGB
            // Be careful!!
            enum { cv_blue = 0, cv_green, cv_red, cv_alpha};
            enum { red = 0, green, blue, alpha};
            if (cn == 4) {
                data[xn*y+x][red] = in.at<cv::Vec4b>(y, x)[cv_red];
                data[xn*y+x][green] = in.at<cv::Vec4b>(y, x)[cv_green];
                data[xn*y+x][blue] = in.at<cv::Vec4b>(y, x)[cv_blue];
                data[xn*y+x][alpha] = in.at<cv::Vec4b>(y, x)[cv_alpha];
            } else {
                data[xn*y+x][red] = in.at<cv::Vec3b>(y, x)[cv_red];
                data[xn*y+x][green] = in.at<cv::Vec3b>(y, x)[cv_green];
                data[xn*y+x][blue] = in.at<cv::Vec3b>(y, x)[cv_blue];
            }
        }
    }
    return out;
}

cv::Mat std_image_to_cv_mat(std_image_type in) {
    cv::Mat out;
    auto & size = std::get<0>(in);
    auto & data = std::get<1>(in);
    int xn;
    int yn;
    xn = size.first;
    yn = size.second;
    out = cv::Mat(cv::Size(xn,yn),CV_8UC3);
    for (int x = 0; x < xn; ++x) {
        for (int y = 0; y < yn; ++y) {
            enum { cv_blue = 0, cv_green, cv_red, cv_alpha};
            enum { red = 0, green, blue, alpha};
            out.at<cv::Vec3b>(y, x)[cv_red] = data[xn*y+x][red];
            out.at<cv::Vec3b>(y, x)[cv_green] = data[xn*y+x][green];
            out.at<cv::Vec3b>(y, x)[cv_blue] = data[xn*y+x][blue];
            // out.at<cv::Vec3b>(y, x)[cv_alpha] = data[xn*y+x][alpha];
        }
    }
    return out;
}
} // namespace segmenter
} // namespace api

#endif // FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_CV_BRIDGE_HPP_INCLUDED
