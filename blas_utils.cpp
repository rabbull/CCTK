//
// Created by karl on 18-11-2.
//

#include <cblas.h>
#include <cmath>
#include <cstring>
#include "blas_utils.h"

void print_dmat(const std::string &name, double *Mat, size_t row, size_t col) {
    printf("%s:\n", name.c_str());
    for (int i = 0; i < row; ++i) {
        printf("\t");
        for (int j = 0; j < col; j ++) {
            auto index = col * i + j;
            printf("%.2f\t", Mat[index]);
        }
        printf("\n");
    }
    printf("\n");
}

double average_dvec(double *vec, size_t size) {
    return cblas_dasum((blasint) size, vec, 1) / size;
}

double p_norm_dvec(int p, double *vec, size_t size) {
    double temp[size];
    memset(temp, 0, size * sizeof(double));
    for (int i = 0; i < size; i ++) {
        temp[i] = pow(vec[i], p);
    }
    double ret = cblas_dasum((blasint) size, temp, 1);
    return pow(ret/size, 1.0/p);
}