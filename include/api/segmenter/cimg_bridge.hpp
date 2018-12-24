#ifndef FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_CIMG_BRIDGE_HPP_INCLUDED
#define FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_CIMG_BRIDGE_HPP_INCLUDED


#include "core.hpp"
#include <cstdint>
#include <iostream>
#define cimg_display 0
#include "../../../submodule/CImg/CImg.h"

namespace api {
namespace segmenter {

std_image_type cimg_to_std_image(const cimg_library::CImg<std::uint8_t> & in) {
    cimg_library::CImg<std::uint8_t> in_;
    std_image_type out;
    auto & size = std::get<0>(out);
    auto & data = std::get<1>(out);
    int xn;
    int yn;
    int zn;
    xn = in.width();
    yn = in.height();
    zn = in.depth();
    size.first = xn;
    size.second = yn;
    data.resize(xn * yn);
    for (int x = 0; x < xn; ++x) {
        for (int y = 0; y < yn; ++y) {
            data[xn*y+x][0] = 0 < zn ? in(x,y,0) : 0;
            data[xn*y+x][1] = 1 < zn ? in(x,y,1) : in(x,y,zn-1);
            data[xn*y+x][2] = 2 < zn ? in(x,y,2) : in(x,y,zn-1);
            data[xn*y+x][3] = 3 < zn ? in(x,y,3) : in(x,y,zn-1);
        }
    }
    return out;
}

cimg_library::CImg<std::uint8_t> std_image_to_cimg(const std_image_type & in) {
    cimg_library::CImg<std::uint8_t> out;
    auto & size = std::get<0>(in);
    auto & data = std::get<1>(in);
    int xn;
    int yn;
    xn = size.first;
    yn = size.second;
    out = cimg_library::CImg<std::uint8_t>();
    // NOTE: channel index is at forth dimension
    // Should not set it to depth (3rd index),
    // so that it could correctly regarded as color
    // and convert to cv::Mat without losing channel information
    out.assign(xn,yn,1,3);
    for (int x = 0; x < xn; ++x) {
        for (int y = 0; y < yn; ++y) {
            out(x,y,0) = data[xn*y+x][0];
            out(x,y,1) = data[xn*y+x][1];
            out(x,y,2) = data[xn*y+x][2];
            // out(x,y,3) = data[xn*y+x][3];
        }
    }
    return out;
}

} // namespace segmenter
} // namespace api

#endif // FINAL_MODULE_SEGMENTER_INCLUDE_API_SEGMENTER_CIMG_BRIDGE_HPP_INCLUDED
