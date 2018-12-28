#include <api/segmenter/cv_cimg_bridge.hpp>
#include <api/segmenter/cv_bridge.hpp>
#include <api/segmenter/segmenter_cimg.hpp>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <chrono>


// taken from 
// https://stackoverflow.com/questions/2808398/easily-measure-elapsed-time
template<typename TimeT = std::chrono::milliseconds>
struct measure
{
    template<typename F, typename ...Args>
    static typename TimeT::rep execution(F&& func, Args&&... args)
        {
            auto start = std::chrono::steady_clock::now();
            std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
            auto duration = std::chrono::duration_cast< TimeT> 
                (std::chrono::steady_clock::now() - start);
            // std::cout << "duration: " << duration.count() << std::endl;
            return duration.count();
        }
};


void write_file(std::string file_path, std::string s) {
    // std::ofstream ofs(file_path, std::ios::binary | std::ios::trunc);
    std::ofstream ofs(file_path, std::ios::binary);
    ofs << s << std::flush;
}

std::string read_file(std::string file_path) {
    std::ifstream ifs(file_path);
    std::string content( (std::istreambuf_iterator<char>(ifs)),
                         (std::istreambuf_iterator<char>()));
    return content;
}

std::vector<std::string> split(
    std::string input,
    std::string s = "\n",
    bool ignore_empty = false)
{
    std::vector<std::string> result;
    auto size = input.size();
    auto npos = std::string::npos;
    std::size_t bpos = 0;
    std::size_t epos = 0;
    while (bpos != size) {
        epos = input.find(s,bpos);
        if (epos == npos) {
            result.push_back(input.substr(bpos,size));
            bpos = epos + s.size();
            break;
        } else {
            result.push_back(input.substr(bpos,epos-bpos));
        }
        bpos = epos + s.size();
    }
    return result;
}

std::vector<std::vector<std::string>> read_csv(std::string file) {
    std::vector<std::vector<std::string>> result;
    std::string content = read_file(file);
    std::vector<std::string> lines = split(content, "\n");
    for (auto line : lines) {
        result.push_back(split(line,","));
    }
    return result;
}

api::segmenter::std_keypoints_type read_csv_keypoints(std::string file) {
    api::segmenter::std_keypoints_type result(2);
    int i = 0;
    auto csv = read_csv(file);
    for (auto record : csv) {
        if (i++) {
            // std::printf(
            //     "%s : (%s,%s)\n",
            //     record[0].c_str(),
            //     record[1].c_str(),
            //     record[2].c_str()
            //     );
            int label = std::stoi(record[0]) ;
            int x = std::stoi(record[1]) ;
            int y = std::stoi(record[2]) ;
            result.resize(label+1);
            result[label].push_back({x,y});
        }
    }
    return result;
}

namespace api {
namespace benchmark {

struct image_statistics {
    std::size_t count;
    std::size_t true_positive;
    std::size_t true_negative;
    std::size_t false_positive;
    std::size_t false_negative;
    double precision;
    double recall;
    double f_measure;
    double mean_absolute_error;
    std::size_t milliseconds;
    std::string to_csv_string() {
        std::string out;
        out += "count,true_positive,true_negative,false_positive,false_negative,precision,recall,f_measure,mean_absolute_error,milliseconds\n";
        out += std::to_string(count) + ","
            + std::to_string(true_positive) + ","
            + std::to_string(true_negative) + ","
            + std::to_string(false_positive) + ","
            + std::to_string(false_negative) + ","
            + std::to_string(precision) + ","
            + std::to_string(recall) + ","
            + std::to_string(f_measure) + ","
            + std::to_string(mean_absolute_error) + ","
            + std::to_string(milliseconds) + "\n"
            ;
        return out;
    }
};

image_statistics calculate_benchmark(
    api::segmenter::std_image_type result,
    api::segmenter::std_image_type ground_truth)
{
    image_statistics statistics{0,0,0,0,0,0,0,0,0,0};
    auto & rs = result;
    auto & gt = ground_truth;
    bool is_true;
    bool is_positive;
    if (std::get<0>(rs) != std::get<0>(gt)) {
        throw std::invalid_argument("size mismatch");
    }
    int xn = std::get<0>(rs).first;
    int yn = std::get<0>(rs).second;
    auto & rsd = std::get<1>(rs); // data
    auto & gtd = std::get<1>(gt); // data
    auto rsf = [&](int x, int y) -> auto& { return rsd[xn*y+x]; };
    auto gtf = [&](int x, int y) -> auto& { return gtd[xn*y+x]; };
    auto is_white_3 = [](auto a) -> bool {return a[0] && a[1] && a[2];};
    auto is_white_aplha = [](auto a) -> bool {return a[3];};
    double mae = 0;
    // int rs_white_count = 0;
    // int gt_white_count = 0;
    for (int x = 0; x < xn; ++x ) {
        for (int y = 0; y < yn; ++y ) {
            // i : intensity
            // n : normalized
            //
            auto & rsi = rsf(x,y);
            auto & gti = gtf(x,y);
            if (is_white_3(rsi) != is_white_aplha(gti))
                ++mae;
            is_true = is_white_3(rsi) == is_white_aplha(gti);
            is_positive = is_white_aplha(gti);
            // if (is_white_3(rsi))
            //     ++rs_white_count;
            // if (is_positive)
            //     ++gt_white_count;
            ++statistics.count;
            if (is_true) {
                if (is_positive) {
                    ++statistics.true_positive;
                } else {
                    ++statistics.true_negative;
                }
            } else {
                if (is_positive) {
                    ++statistics.false_positive;
                } else {
                    ++statistics.false_negative;
                }
            }
        }
    }
    int n = statistics.count;
    int tp = statistics.true_positive;
    int fp = statistics.false_positive;
    int tn = statistics.true_negative;
    int fn = statistics.false_negative;
    double pre = statistics.precision = 1. * tp / (0. + tp + fp);
    double rec = statistics.recall = 1. * tp / (0. + tp + fn);
    statistics.f_measure = 2 * pre * rec / (pre + rec);
    mae /= n;
    statistics.mean_absolute_error = mae;
    // std::cout << "rs_white: " << rs_white_count << std::endl;
    // std::cout << "gt_white: " << gt_white_count << std::endl;
    return statistics;
}


}
}

api::segmenter::std_image_type
apply_mask(
api::segmenter::std_image_type image,
api::segmenter::std_image_type mask) {
    api::segmenter::std_image_type result;
    if (std::get<0>(image) != std::get<0>(mask)) {
        throw std::invalid_argument("size mismatch");
    }
    auto size = std::get<0>(image);
    std::get<0>(result) = size;
    auto & src = std::get<1>(image);
    auto & dst = std::get<1>(result);
    dst.resize(src.size());
    auto p = std::get<1>(mask).begin();
    std::transform(src.begin(), src.end(), dst.begin(),
                   [&](auto i){
                       auto& d = *p;
                       ++p;
                       if (d[0]&&d[1]&&d[2]) {
                           return i;
                       } else {
                           return std::array<std::uint8_t,4>{0,0,0,0};
                       }
                    });
    return result;
}

api::segmenter::std_image_type
mark_keypoints(
    api::segmenter::std_image_type image,
    api::segmenter::std_keypoints_type keypoints) {
    cv::Mat out = api::segmenter::std_image_to_cv_mat(image);
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
    return api::segmenter::cv_mat_to_std_image(out);
}

int main(int argc, char** argv) {
    try {
        if (argc < 4) {
            return -1;
        }
        const char * image_path = argv[1];
        const char * keypoints_csv_path = argv[2];
        const char * output_image_path = argv[3];

        auto image = api::segmenter::cv_mat_to_std_image(cv::imread(image_path));
        auto keypoints = read_csv_keypoints(keypoints_csv_path);
        api::segmenter::segmenter_cimg segmenter;
        api::segmenter::std_image_type output_image;
        auto segment = [&](){
                           output_image = segmenter.segment(image, keypoints);
                       };

        auto duration = measure<>::execution(segment);
        auto output_cv_image = api::segmenter::std_image_to_cv_mat(output_image);
        cv::imwrite(output_image_path, output_cv_image);
        if (argc < 5) {
            return 0;
        }
        const char * output_figure_path = argv[4];
        auto figure_image = apply_mask(image, output_image);
        auto figure_cv_image = api::segmenter::std_image_to_cv_mat(figure_image);
        cv::imwrite(output_figure_path, figure_cv_image);
        if (argc < 6) {
            return 0;
        }
        const char * output_marked_figure_path = argv[5];
        auto figure_marked_image = mark_keypoints(figure_image, keypoints);
        auto figure_marked_cv_image = api::segmenter::std_image_to_cv_mat(figure_marked_image);
        cv::imwrite(output_marked_figure_path, figure_marked_cv_image);
        if (argc < 8) {
            return 0;
        }
        const char * ground_truth_path = argv[6];
        const char * output_benchmark_csv_path = argv[7];
        auto gt = cv::imread(ground_truth_path,cv::IMREAD_UNCHANGED);
        // auto gt = cv::imread(ground_truth_path);
        // std::cout << gt.channels() << std::endl;
        // cv::imshow("img",output_image);
        // cv::imshow("gt",gt);
        // cv::waitKey();
        auto gt_image = api::segmenter::cv_mat_to_std_image(gt);
        auto s = api::benchmark::calculate_benchmark(output_image, gt_image);
        s.milliseconds = duration;
        // std::cout << "duration2: " << duration << std::endl;
        // std::cout << s.to_csv_string() << std::endl;
        // std::cout << s.to_csv_string();
        write_file(output_benchmark_csv_path, s.to_csv_string());
    } catch(const std::exception & e){
        std::cerr << "exception catched: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
