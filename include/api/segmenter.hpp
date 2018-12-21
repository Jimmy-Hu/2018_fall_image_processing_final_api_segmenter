#ifndef FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_HPP_INCLUDED
#define FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_HPP_INCLUDED

#include <algorithm>

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
    xn = in.cols;
    yn = in.rows;
    size.first = xn;
    size.second = yn;
    data.resize(in.rows * in.cols);
    for (int x = 0; x < xn; ++x) {
        for (int y = 0; y < yn; ++y) {
            data[xn*y+x][0] = in.at<cv::Vec3b>(y, x)[0];
            data[xn*y+x][1] = in.at<cv::Vec3b>(y, x)[1];
            data[xn*y+x][2] = in.at<cv::Vec3b>(y, x)[2];
            data[xn*y+x][3] = 255;
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
            out.at<cv::Vec3b>(y, x)[0] = data[xn*y+x][0];
            out.at<cv::Vec3b>(y, x)[1] = data[xn*y+x][1];
            out.at<cv::Vec3b>(y, x)[2] = data[xn*y+x][2];
        }
    }
    return out;
}


}}
#endif // FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_HPP_INCLUDED
