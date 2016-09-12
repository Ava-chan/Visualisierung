#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <vector>
#include <utility>
#include <math.h>


class Interpolator : private std::vector<std::pair<double, double> >
{
public:
    void add(double value, double distance);
    double operator()();

    virtual ~Interpolator() {};

protected:
    virtual double weightFunction(double distance) = 0;
};

#endif // IINTERPOLATOR
