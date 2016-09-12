#include "Interpolator.h"

#include <iostream>
using namespace std;


void Interpolator::add(double value, double distance)
{
    push_back(pair<double, double>(value, distance));
}


double Interpolator::operator()()
{
    double numerator = 0.0f;
    double denominator = 0.0f;

    for (size_t i=0; i<size(); ++i)
    {
        double value = at(i).first;
        double distance = at(i).second;

        numerator += (weightFunction(distance) * value);
        denominator += weightFunction(distance);
    }
    return (numerator / denominator);
}
