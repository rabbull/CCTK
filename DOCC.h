//
// Created by karl on 18-11-3.
//

#ifndef CCTK_DOCC_H
#define CCTK_DOCC_H

#include <Magick++/Geometry.h>
#include <cblas.h>
#include "types.h"
#include "const.h"
#include "traditional_fuctions.h"

inline const int MAX_POOLING = 1;
inline const int AVG_POOLING = 2;

void docc(pctype input[], const Magick::Geometry &size, double sigma, size_t kernel_size, double k, double lambda = 3,
          int pooling_method = MAX_POOLING);


#endif //CCTK_DOCC_H
