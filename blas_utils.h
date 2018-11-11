//
// Created by karl on 18-11-2.
//

#ifndef CCTK_BLAS_UTILS_H
#define CCTK_BLAS_UTILS_H

#include <cstdio>
#include <string>

void print_dmat(const std::string &name, double *Mat, size_t row, size_t col);

double average_dvec(double *vec, size_t size);

double p_norm_dvec(int p, double *vec, size_t size);

#endif //CCTK_BLAS_UTILS_H
