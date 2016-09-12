#include "VisHelper.h"

namespace VisHelper
{
    double euclideanDistance(fantom::Tensor<double, 2> p1,
                             fantom::Tensor<double, 2> p2)
    {
        return std::sqrt(std::pow(p1[0] - p2[0], 2.0f) + std::pow(p1[1] - p2[1], 2.0));
    }
}

