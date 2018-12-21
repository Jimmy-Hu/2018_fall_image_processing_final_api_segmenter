#ifndef FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_SEGMENTER_CIMG_HPP_INCLUDED
#define FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_SEGMENTER_CIMG_HPP_INCLUDED

#include "core.hpp"
#include <exception>
// #include "cv_cimg_bridge.hpp"
// #include "cv_bridge.hpp"
#include "cimg_bridge.hpp"
// #include <opencv/cv.h>

namespace api {
namespace segmenter {

class segmenter_cimg {
public:
    segmenter_cimg() = default;
    segmenter_cimg(segmenter_cimg const &) = default;
    segmenter_cimg(segmenter_cimg &&) = default;
    ~segmenter_cimg() = default;
    std_image_type segment(const std_image_type & source, std_keypoints_type keypoints) {
        using namespace ::cimg_library;
        if (std::get<0>(source).first <= 0 || std::get<0>(source).second <=0) {
            throw std::invalid_argument("invalid source-image size");
        }
        const CImg<std::uint8_t> image(std_image_to_cimg(source));
        // const CImg<std::uint8_t> image(std_image_to_cv_mat(source));
        // const CImg<std::uint8_t> image(cv::imread("../dataset/4.jpeg"));
        cv::imshow("image", image.get_MAT());

        // cv::Mat label_mask = cv::imread("../dataset/kp4.png");
        // cv::Mat label(cv::Size(label_mask.col, label_mask.row));
        // Load input label image.
        CImg<std::uint8_t> labels(image.width(),image.height(),1,1,0);
        int xn = image.width();
        int yn = image.height();
        int x;
        int y;
        int label = 0;
        for (const auto & layer : keypoints) {
            ++label;
            for (const auto & pt : layer) {
                x = pt.first;
                y = pt.second;
                if (x < 0 || x >= xn || y < 0 || y >= yn) {
                    throw std::invalid_argument("keypoint out of range");
                }
                labels(x,y,0) = label;
                // labels(x,y,1) = label;
                // labels(x,y,2) = label;
            }
        }
        labels.resize(image.width(),image.height(),1,1,0);  // Be sure labels has the correct size.
        // labels.resize(image.width(),image.height(),1,1,0);  // Be sure labels has the correct size.
        // Compute gradients.
        const float sigma = 0.002f * std::max(image.width(),image.height());
        const CImg<std::uint8_t> blurred = image.get_blur(sigma);
        CImgList<std::uint8_t> gradient = blurred.get_gradient("xy");
        // gradient[0] and gradient[1] are two CImg<std::uint8_t> images which contain
        // respectively the X and Y derivatives of the blurred RGB image.
        // Compute the potential map P.
        CImg<float> P(labels);
        cimg_forXY(P,x,y){
            P(x,y) = 1./(1. +
                         cimg::sqr(gradient(0,x,y,0)) + // Rx^2
                         cimg::sqr(gradient(0,x,y,1)) + // Gx^2
                         cimg::sqr(gradient(0,x,y,2)) + // Bx^2
                         cimg::sqr(gradient(1,x,y,0)) + // Ry^2
                         cimg::sqr(gradient(1,x,y,1)) + // Gy^2
                         cimg::sqr(gradient(1,x,y,2))); // By^2
        }

        // Run the label propagation algorithm.
        labels.watershed(P, true);
        cimg_forXY(labels,x,y){
            auto label = labels(x,y);
            enum { foreground = 1};
            labels(x,y) = (label != 2) ? 255 : 0;
        }

        CImg<std::uint8_t> P2(P);
        cimg_forXY(P,x,y){
            P2(x,y) = P(x,y)*255;
        }
        cv::imshow("p", P2.get_MAT());
        cv::imshow("gx",  gradient.at(0).get_MAT());
        cv::imshow("gy",  gradient.at(1).get_MAT());

        return cimg_to_std_image(labels);
    }
};

} // namespace segmenter
} // namespace api

#endif // FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_SEGMENTER_CIMG_HPP_INCLUDED
