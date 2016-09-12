#ifndef VISHELPER_H
#define VISHELPER_H

#include <fantom/algorithm.hpp>
#include <fantom/fields.hpp>
#include <fantom/register.hpp>

#include <cmath>
#include <utility>

namespace VisHelper
{
    double euclideanDistance(fantom::Tensor<double, 2> p1,
                             fantom::Tensor<double, 2> p2);
}

#endif // VISHELPER_H

