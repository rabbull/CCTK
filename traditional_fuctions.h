//
// Created by karl on 18-11-2.
//

#ifndef CCTK_TRADITIONAL_FUCTIONS_H
#define CCTK_TRADITIONAL_FUCTIONS_H

#include "types.h"
#include "const.h"
#include <Magick++.h>

namespace wb {
    void gray_world(pctype color[], const Magick::Geometry &size);

    void white_patch(pctype color[], const Magick::Geometry &size);

    void max_RGB(pctype color[], const Magick::Geometry &size);

    void SoG(pctype color[], const Magick::Geometry &size, int p = 6);

    void ACE(pctype color[], const Magick::Geometry &size);
}
#endif //CCTK_TRADITIONAL_FUCTIONS_H
