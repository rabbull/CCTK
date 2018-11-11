//
// Created by karl on 18-11-2.
//

#include <cblas.h>
#include <cstring>
#include <exception>
#include "traditional_fuctions.h"
#include "blas_utils.h"

void wb::gray_world(pctype color[], const Magick::Geometry &size) {
    SoG(color, size, 1);
}

void wb::white_patch(pctype color[], const Magick::Geometry &size) {
    max_RGB(color, size);
}
void wb::max_RGB(pctype color[], const Magick::Geometry &size) {
    ctype avg[DEBUG_ENOUGH_SIZE];
    double scale[DEBUG_ENOUGH_SIZE];

    size_t height = size.height();
    size_t width = size.width();
    size_t area = height * width;

    avg[B] = cblas_idamax((blasint) area, color[B], 1);
    avg[G] = cblas_idamax((blasint) area, color[G], 1);
    avg[R] = cblas_idamax((blasint) area, color[R], 1);
    ctype gray_value = average_dvec(avg, 3);
    for (int i = 0; i < 3; i++) {
        scale[i] = gray_value / avg[i];
    }
    cblas_dscal((blasint) area, scale[B], color[B], 1);
    cblas_dscal((blasint) area, scale[G], color[G], 1);
    cblas_dscal((blasint) area, scale[R], color[R], 1);
}

void wb::SoG(pctype color[], const Magick::Geometry &size, int p) {
    ctype avg[DEBUG_ENOUGH_SIZE];
    double scale[DEBUG_ENOUGH_SIZE];

    size_t height = size.height();
    size_t width = size.width();
    size_t area = height * width;

    avg[B] = p_norm_dvec(p, color[B], area);
    avg[R] = p_norm_dvec(p, color[R], area);
    avg[G] = p_norm_dvec(p, color[G], area);
    ctype gray_value = average_dvec(avg, 3);
    for (int i = 0; i < 3; i++) {
        scale[i] = gray_value / avg[i];
    }
    cblas_dscal((blasint) area, scale[B], color[B], 1);
    cblas_dscal((blasint) area, scale[G], color[G], 1);
    cblas_dscal((blasint) area, scale[R], color[R], 1);
}

void wb::ACE(pctype *color, const Magick::Geometry &size) {
    throw std::exception();
}