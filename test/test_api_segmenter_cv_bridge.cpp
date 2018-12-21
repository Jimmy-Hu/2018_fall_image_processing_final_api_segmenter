#include <api/segmenter/cv_bridge.hpp>
#include <algorithm>

int main() {
    cv::Mat a(cv::Size{123,321}, CV_8UC3, cv::Scalar{0,1,0});
    api::segmenter::std_image_type b = api::segmenter::cv_mat_to_std_image(a);
    cv::Mat c = api::segmenter::std_image_to_cv_mat(b);
    api::segmenter::std_image_type d = api::segmenter::cv_mat_to_std_image(a);
    bool success = true;
    success &= std::get<0>(b) == std::get<0>(d);
    // std::cerr << "size same assertion: "
    success &= std::equal(std::get<1>(b).begin(), std::get<1>(b).end(), std::get<1>(d).begin());
    return success ? 0 : -1;
}
