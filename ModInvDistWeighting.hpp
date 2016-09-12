#ifndef MODIFIED_INVERSE_DISTANCE_WEIGHTING_H
#define MODIFIED_INVERSE_DISTANCE_WEIGHTING_H

#include <vector>
#include <utility>
#include <math.h>

#include "Interpolator.h"


class ModifiedInverseDistanceWeighting : public Interpolator
{    
    double R=0.5;
    unsigned int p=8;

    double weightFunction(double distance)
    {
	return (R-distance > 0) ? pow((R-distance) / (R*distance), p) : 0.0f;
    }
};

#endif
