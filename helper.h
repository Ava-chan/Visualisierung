#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>

class fileNotFound {};

std::int64_t SToLL(const std::string& sStrIn);
float SToF(const std::string& sStrIn);

bool EndsWith(std::string const &oString,
              std::string const &oEnding);

double RadToDeg(double fRad);
double DegToRad(double fDeg);

bool Equals(double fA, double fB, double fPrecision = 0.005);
bool EQPercentageDiff(double fA, double fB, double fPrecision = 0.01);

#endif // HELPER_H
