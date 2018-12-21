#include <api/segmenter/cv_cimg_bridge.hpp>
#include <api/segmenter/segmenter_cimg.hpp>
#include <api/segmenter/cv_bridge.hpp>
#include <opencv/cv.h>

using namespace cimg_library;

using namespace api::segmenter;
int main() {
    api::segmenter::segmenter_cimg s;
    cv::Mat in = cv::imread("../dataset/4.jpeg");

    std_image_type in_ = cv_mat_to_std_image(in);
    std_keypoints_type keypoints =
        {
            // foreground
            {
                {183,683},
                {187,406},
                {212,714},
                {263,854},
                {268,789},
                {272,295},
                {280,708},
                {310,453},
                {322,262},
                {325,158},
                {331,578},
                {356,496},
                {368,115},
                {369,283},
                {371,773},
                {373,310},
                {375,207},
                {387,691},
                {397,418},
                {435,239},
                {442,540},
                {447,528},
                {466,730},
                {481,206},
                {490,606},
                {501,726},
                {503,527},
                {506,851},
                {507,498},
                {80,565}
            },
            // background
            {
                // {0,0},
                {148,260},
                {156,733},
                {173,578},
                {184,517},
                {212,648},
                {304,47},
                {519,103},
                {539,241},
                {557,460},
                {557,640},
                {588,774},
                {616,292},
                {646,582},
                {71,643}
            }
        };
    segmenter_cimg seg;
    std_image_type out_ = seg.segment(in_, keypoints);
    cv::Mat out = std_image_to_cv_mat(out_);
    // foreach keypoint in background
    for(auto pt_ : keypoints[0]) {
        cv::Point pt(pt_.first, pt_.second);
        cv::circle(out, pt, 3, cv::Scalar{0,255,0}, -1);
    }
    // foreach keypoint in background
    for(auto pt_ : keypoints[1]) {
        cv::Point pt(pt_.first, pt_.second);
        cv::circle(out, pt, 3, cv::Scalar{0,0,255}, -1);
    }

    cv::imshow("in", in);
    cv::imshow("out", out);
    cv::waitKey();
    return 0;
}
