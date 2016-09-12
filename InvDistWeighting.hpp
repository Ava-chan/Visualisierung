#ifndef INVERSE_DISTANCE_WEIGHTING_H
#define INVERSE_DISTANCE_WEIGHTING_H

#include <vector>
#include <utility>
#include <math.h>

#include "Interpolator.h"


class InverseDistanceWeighting : public Interpolator
{    
    unsigned int p=8;

    double weightFunction(double distance)
    {
        return 1.0f / pow(distance, p);
    }
};

#endif
