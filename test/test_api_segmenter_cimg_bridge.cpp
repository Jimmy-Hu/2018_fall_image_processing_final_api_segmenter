#include <api/segmenter/cv_cimg_bridge.hpp>
#include <api/segmenter/cimg_bridge.hpp>

using namespace cimg_library;

int main() {
    CImg<unsigned char> a(2,2,3,1,
                          1,2,3,4,
                          2,2,3,4,
                          3,2,3,4);
    CImg<unsigned char> a2(2,2,3,1,
                          1,2,3,0,
                          2,2,3,0,
                          3,2,3,0);
    api::segmenter::std_image_type b = api::segmenter::cimg_to_std_image(a);
    CImg<unsigned char> c = api::segmenter::std_image_to_cimg(b);
    bool success = true;
    success &= a == c && c != a2;
    std::cerr << std::boolalpha << "success=" << success << std::endl;
    return success ? 0 : -1;
}
