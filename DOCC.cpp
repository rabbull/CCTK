//
// Created by karl on 18-11-3.
//

#include "DOCC.h"

/**
 * return a matrix that is the convolution-by-gauss of the given one
 * @param matrix
 * @param new_matrix
 * @param matrx_size
 * @param kernel_size
 * @attention kernel_size have to be odd, but if not, thought it will go wrong, no exception will be thrown.
 */
void convolute_with_gauss(const double *, double *, const Magick::Geometry &, const double, const size_t);

/**
 * 2-d gauss function
 * @param x
 * @param y
 * @param sigma
 * @return retval
 */
double gauss(double, double, double);

/**
 * gives a gauss kernel
 * @param sigma
 * @param size
 * @return the gauss kernel
 * @attention remember to release the kernel
 */
double *gauss_kernel(double, size_t);

void docc(pctype input[], const Magick::Geometry &size, double sigma, size_t kernel_size, double k, double lambda,
          int pooling_method) {
    size_t height = size.height();
    size_t width = size.width();
    size_t area = height * width;
    double *RGB2LMS[3];
    double RGB2L[3] = {.0447, .6098, .3192};
    double RGB2M[3] = {.0870, .7638, .1647};
    double RGB2S[3] = {.9391, .1296, .0202};
    const int L = 0;
    const int M = 1;
    const int S = 2;
    RGB2LMS[L] = RGB2L;
    RGB2LMS[M] = RGB2M;
    RGB2LMS[S] = RGB2S;

    // cone cell layer
    pctype LMS[3] = {nullptr};
    for (auto &channel : LMS) {
        channel = new ctype[area];
    }

    // convert RGB 2 LMS
    for (int i = 0; i < 3; i++) {
        cblas_dcopy((blasint) area, input[R], 1, LMS[i], 1);
        cblas_daxpby((blasint) area, RGB2LMS[i][G], input[G], 1, RGB2LMS[i][R], LMS[i], 1);
        cblas_daxpby((blasint) area, RGB2LMS[i][B], input[B], 1, 1, LMS[i], 1);
    }

    // copy RGB 2 LMS
//    cblas_dcopy((blasint) area, input[R], 1, LMS[L], 1);
//    cblas_dcopy((blasint) area, input[G], 1, LMS[M], 1);
//    cblas_dcopy((blasint) area, input[B], 1, LMS[S], 1);

    int LM = 0, ML = 1, YS = 2, SY = 3, BP = 4, BN = 5;

    // LGN layer
    pctype O[6];
    for (auto &e : O) {
        e = new ctype[area];
    }

    // O_LM
    cblas_dcopy((blasint) area, LMS[L], 1, O[LM], 1);
    cblas_daxpy((blasint) area, -1.0, LMS[M], 1, O[LM], 1);
    cblas_dscal((blasint) area, sqrt(.5), O[LM], 1);

    // O_ML
    cblas_daxpy((blasint) area, -1, O[LM], 1, O[ML], 1);

    // O_YS
    cblas_dcopy((blasint) area, LMS[L], 1, O[YS], 1);
    cblas_daxpy((blasint) area, 1, LMS[M], 1, O[YS], 1);
    cblas_daxpy((blasint) area, -2, LMS[S], 1, O[YS], 1);
    cblas_dscal((blasint) area, sqrt(1.0 / 6.0), O[YS], 1);

    // O_SY
    cblas_daxpy((blasint) area, -1, O[YS], 1, O[SY], 1);

    // O_BP
    cblas_dcopy((blasint) area, LMS[L], 1, O[BP], 1);
    cblas_daxpy((blasint) area, 1, LMS[M], 1, O[BP], 1);
    cblas_daxpy((blasint) area, 1, LMS[S], 1, O[BP], 1);
    cblas_dscal((blasint) area, sqrt(1.0 / 3.0), O[BP], 1);

    // O_BN
    cblas_daxpy((blasint) area, -1, O[BP], 1, O[BN], 1);

    // V1 layer
    int _LM = 0;
    int _SY = 1;
    int _B = 2;
    pctype DO[3];

    for (auto &i : DO) {
        i = new ctype[area];
    }

    auto SO_1 = new ctype[area];
    auto SO_2 = new ctype[area];

    // DO_lm
    convolute_with_gauss(O[LM], SO_1, size, sigma, kernel_size);
    convolute_with_gauss(O[ML], SO_2, size, lambda * sigma, kernel_size);
    cblas_dcopy((blasint) area, SO_1, 1, DO[_LM], 1);
    cblas_daxpy((blasint) area, k, SO_2, 1, DO[_LM], 1);

    // DO_sy
    convolute_with_gauss(O[SY], SO_1, size, sigma, kernel_size);
    convolute_with_gauss(O[YS], SO_2, size, lambda * sigma, kernel_size);
    cblas_dcopy((blasint) area, SO_1, 1, DO[_SY], 1);
    cblas_daxpy((blasint) area, k, SO_2, 1, DO[_SY], 1);

    // DO_b
    convolute_with_gauss(O[BP], SO_1, size, sigma, kernel_size);
    convolute_with_gauss(O[BN], SO_2, size, lambda * sigma, kernel_size);
    cblas_dcopy((blasint) area, SO_1, 1, DO[_B], 1);
    cblas_daxpy((blasint) area, k, SO_2, 1, DO[_B], 1);


    // V4 layer
    double invSqrt2 = sqrt(1 / 2.0);
    double invSqrt3 = sqrt(1 / 3.0);
    double invSqrt6 = invSqrt2 * invSqrt3;

    pctype DT[3];
    DT[L] = SO_1;
    DT[M] = SO_2;
    DT[S] = new ctype[area];

    // DT_L
    cblas_dcopy((blasint) area, DO[_LM], 1, DT[L], 1);
    cblas_daxpy((blasint) area, invSqrt3, DO[_SY], 1, DT[L], 1);
    cblas_daxpby((blasint) area, invSqrt3, DO[_B], 1, invSqrt2, DT[L], 1);

    // DT_M
    cblas_dcopy((blasint) area, DO[_LM], 1, DT[M], 1);
    cblas_daxpby((blasint) area, invSqrt3, DO[_SY], 1, -1, DT[M], 1);
    cblas_daxpby((blasint) area, invSqrt3, DO[_B], 1, invSqrt2, DT[M], 1);

    // DT_S
    cblas_dcopy((blasint) area, DO[_SY], 1, DT[S], 1);
    cblas_daxpby((blasint) area, invSqrt3, DO[_B], 1, -2 * invSqrt6, DT[S], 1);


    double LMS2R[3] = {5.3341, -4.2829, 0.1428};
    double LMS2G[3] = {-1.1556, 2.2581, -0.1542};
    double LMS2B[3] = {0.0448, -0.2195, 1.0831};

    cblas_dcopy((blasint) area, DT[L], 1, input[R], 1);
    cblas_daxpby((blasint) area, LMS2R[M], DT[M], 1, LMS2R[L], input[R], 1);
    cblas_daxpy((blasint) area, LMS2R[S], DT[S], 1, input[R], 1);

    cblas_dcopy((blasint) area, DT[L], 1, input[G], 1);
    cblas_daxpby((blasint) area, LMS2G[M], DT[M], 1, LMS2G[L], input[G], 1);
    cblas_daxpy((blasint) area, LMS2G[S], DT[S], 1, input[G], 1);

    cblas_dcopy((blasint) area, DT[L], 1, input[B], 1);
    cblas_daxpby((blasint) area, LMS2B[M], DT[M], 1, LMS2B[L], input[B], 1);
    cblas_daxpy((blasint) area, LMS2B[S], DT[S], 1, input[B], 1);

    return;

    double (*pooling)(double *, size_t);
    switch (pooling_method) {
        case MAX_POOLING:
            pooling = [](double *vec, size_t size) -> double {
                return vec[cblas_idamax((blasint) size, vec, 1)];
            };
            break;
        case AVG_POOLING:
            pooling = [](double *vec, size_t size) -> double {
                return cblas_dasum((blasint) size, vec, 1) / size;
            };
            break;
        default:
            fprintf(stderr, "[ERR] UNKNOWN POOLING TYPE");
            exit(EXIT_FAILURE);
    }
    double p[3] = {0}, e[3] = {0}, coef = 0;
    for (int i = 0; i < 3; i++) {
        p[i] = pooling(DT[i], area);
        coef += p[i];
    }

    cblas_dcopy(3, p, 1, e, 1);
    cblas_dscal(3, 1 / coef, e, 1);

//
//    double r = max(cblas_ddot(3, LMS2R, 1, e, 1), 0);
//    double g = max(cblas_ddot(3, LMS2G, 1, e, 1), 0);
//    double b = max(cblas_ddot(3, LMS2B, 1, e, 1), 0);

    cblas_dscal((blasint) 3, 3, e, 1);
    for (double &i : e) {
        i = 1 / i;
    }
    for (int i = 0; i < 3; i ++) {
        cblas_dscal((blasint) area, e[i], LMS[i], 1);
    }
    cblas_dcopy((blasint) area, LMS[L], 1, input[R], 1);
    cblas_daxpby((blasint) area, LMS2R[M], LMS[M], 1, LMS2R[L], input[R], 1);
    cblas_daxpy((blasint) area, LMS2R[S], LMS[S], 1, input[R], 1);

    cblas_dcopy((blasint) area, LMS[L], 1, input[G], 1);
    cblas_daxpby((blasint) area, LMS2G[M], LMS[M], 1, LMS2G[L], input[G], 1);
    cblas_daxpy((blasint) area, LMS2G[S], LMS[S], 1, input[G], 1);

    cblas_dcopy((blasint) area, LMS[L], 1, input[B], 1);
    cblas_daxpby((blasint) area, LMS2B[M], LMS[M], 1, LMS2B[L], input[B], 1);
    cblas_daxpy((blasint) area, LMS2B[S], LMS[S], 1, input[B], 1);

    for (auto &channel : LMS) {
        delete[] channel;
    }
    for (auto &item : O) {
        delete[] item;
    }
    for (auto &channel : DT) {
        delete[] channel;
    }
}

double gauss(double x, double y, double sigma) {
    return 1 / (2 * M_PI * sigma * sigma) * pow(M_E, (-(x * x + y * y) / 2 / sigma / sigma));
}

double *gauss_kernel(const double sigma, const size_t size) {
    auto ret = new double[size * size];
    double sum = 0;
    if (size & 1u) { // size is odd
        double mid = size >> 1;
        for (unsigned i = 0; i < size; i++) {
            for (unsigned j = 0; j < size; j++) {
                auto index = i * size + j;
                ret[index] = gauss(i - mid, j - mid, sigma);
                sum += ret[index];
            }
        }
    } else { // size is even
        double mid = (size - 1) / 2.0;
        for (unsigned i = 0; i < size; i++) {
            for (unsigned j = 0; j < size; j++) {
                auto index = i * size + j;
                ret[index] = gauss(i - mid, j - mid, sigma);
                sum += ret[index];
            }
        }
    }
    double k = 1 / sum;
    for (int i = 0; i < size * size; i++) {
        ret[i] *= k;
    }
    return ret;
}

void convolute_with_gauss(const double *data, double *ret, const Magick::Geometry &size, const double sigma,
                          const size_t kernel_size) {
    auto height = size.height();
    auto width = size.width();

    auto kernel = gauss_kernel(sigma, kernel_size);

    auto half_ksize = (int) (kernel_size >> 1);

    for (int _y = 0; _y < height; ++_y) {
        for (int _x = 0; _x < width; ++_x) {
            auto m_start = _y - half_ksize;
            auto m_end = _y + half_ksize;
            auto n_start = _x - half_ksize;
            auto n_end = _x + half_ksize;
            auto index = _y * width + _x;
            for (int m = m_start; m <= m_end; ++m) {
                for (int n = n_start; n <= n_end; ++n) {
                    if (m >= 0 && n >= 0) {
                        ret[index] +=
                                data[index] * kernel[(m + half_ksize - _y) * kernel_size + n + half_ksize - _x];
                    }
                }
            }
        }
    }
    delete[] kernel;
}