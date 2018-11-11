#include <cblas.h>
#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/getopt_core.h>
#include "blas_utils.h"
#include "types.h"
#include "const.h"
#include "traditional_fuctions.h"
#include "DOCC.h"

void init_color_channel(const Magick::Image &raw, pctype color_channel[], const Magick::Geometry &size) {
    auto h = size.height();
    auto w = size.width();
    auto s = h * w;
    color_channel[B] = (pctype) malloc(s * sizeof(ctype));
    color_channel[G] = (pctype) malloc(s * sizeof(ctype));
    color_channel[R] = (pctype) malloc(s * sizeof(ctype));
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            auto curr_pixel = raw.pixelColor(i, j);
            auto index = i * h + j;
            color_channel[B][index] = ((ctype) curr_pixel.quantumBlue());
            color_channel[G][index] = ((ctype) curr_pixel.quantumGreen());
            color_channel[R][index] = ((ctype) curr_pixel.quantumRed());
        }
    }
}

void output_picture(const Magick::Geometry &size, const std::string &output_file, pctype channel[]) {
    Magick::Image output(size, "white");
    auto w = size.width();
    auto h = size.height();
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            Magick::Color curr_pixel;
            auto index = i * h + j;
            curr_pixel.quantumBlue((float) channel[B][index]);
            curr_pixel.quantumGreen((float) channel[G][index]);
            curr_pixel.quantumRed((float) channel[R][index]);
            curr_pixel.quantumAlpha(65535.0f);
            output.pixelColor(i, j, curr_pixel);
        }
    }
    output.syncPixels();
    output.write(output_file);
}

void single_picture(char *file) {
    Magick::Image raw_input;
    raw_input.read(std::string(file));
    auto size = raw_input.size();
    size_t h = 0, w = 0, s = 0;
    h = size.height();
    w = size.width();
    s = h * w;
    pctype color_channel[DEBUG_ENOUGH_SIZE];
    double sigma = 3.0;

    int code = mkdir("cctk_output", S_IRWXU | S_IRGRP | S_IROTH | S_IXGRP | S_IXOTH);
    if (code) {

    }

    init_color_channel(raw_input, color_channel, size);
    wb::max_RGB(color_channel, size);
    output_picture(size, "cctk_output/maxRGB.bmp", color_channel);

//    init_color_channel(raw_input, color_channel, size);
//    wb::SoG(color_channel, size, 6);
//    output_picture(size, "SoG-6.bmp", color_channel);
//
//    init_color_channel(raw_input, color_channel, size);
//    wb::gray_world(color_channel, size);
//    output_picture(size, "GrayWorld.bmp", color_channel);
//
//    init_color_channel(raw_input, color_channel, size);
//    for (int i = 0; i < 1; i++) {
//        docc(color_channel, size, sigma, 121, .3);
//    }
//    output_picture(size, std::string("DOCC-") + std::to_string(100) + std::string(".bmp"), color_channel);

    for (auto &p : color_channel) {
        if (!p) {
            free(p);
        }
    }
}

int main(int argc, char **argv) {
//    if (argc <= 1) {
//        std::cerr << "[ERROR] 请输入至少一个参数" << std::endl;
//        std::cout << "用法: cctk picture.jpg" << std::endl;
//        std::cout << "cctk会在图片当前目录建立一个子目录，用来存储对所输入的图像的处理结果。"
//                  << "处理结果包括四张图片，分别为经 maxRGB (ata. white-patch), SoG-6, gray-world 和 DOCC 这四种算法的处理的结果。" << std::endl;
//    } else {
//        for (auto i = 0; i < argc; ++i) {
//            single_picture(argv[i]);
//        }
//    }

    int c;
//
//    const int OUTPUT = 0;
//    const int FORMAT = 1;
    enum {
        OUTPUT,
        FORMAT
    };
    bool opt_flags[10] = {false};
    char *opt_value[10] = {nullptr};

    opterr = 0;
    while ((c = getopt(argc, argv, "o:f:h")) != -1) {
        switch (c) {
            case 'o':
                opt_flags[OUTPUT] = false;
                std::cout << R"([INFO] option 'o' is selected.)" << std::endl;
                opt_value[OUTPUT] = optarg;
                std::cout << R"([INFO] option 'o' caught ")"
                          << std::string(opt_value[OUTPUT])
                          << R"(" as its argument.)" << std::endl;
                break;

            case 'f':
                opt_flags[FORMAT] = false;
                std::cout << R"([INFO] option 'f' is selected.)" << std::endl;
                opt_value[FORMAT] = optarg;
                std::cout << R"([INFO] option 'o' caught ")"
                          << std::string(opt_value[FORMAT])
                          << R"(" as its argument.)" << std::endl;
                break;

            case '?':
            case 'h':
                if (optopt == 'o') {
                    std::cerr << R"([ERROR] "-o" requires an argument but not detected.)" << std::endl;
                } else if (optopt == 'f') {
                    std::cerr << R"([ERROR] "-f" requires an argument but not detected.)" << std::endl;
                } else {
                    std::cerr << R"([ERROR] Unknown option: )" << (char) optopt << std::endl;
                }

                std::cout << "Usage: cctk img.png -o img.jpg" << std::endl;
                exit(EXIT_FAILURE);

            default:
                std::cerr << R"([ERROR] "getopt" failed!)" << std::endl;
                abort();
        }
    }

    return 0;
}