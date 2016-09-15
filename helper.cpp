#include "helper.h"


std::int64_t SToLL(const std::string& sStrIn)
{
    std::int64_t nLL;
    std::stringstream sStrOut(sStrIn);
    sStrOut >> nLL;
    return nLL;
}


float SToF(const std::string& sStrIn)
{
    float fF;
    std::stringstream sStrOut(sStrIn);
    sStrOut >> fF;
    return fF;
}


bool EndsWith(std::string const &oString, std::string const &oEnding)
 {
     if (oEnding.size() > oString.size())
     {
         return false;
     }

     return std::equal(oEnding.rbegin(),
                       oEnding.rend(),
                       oString.rbegin());
 }


double RadToDeg(double fRad)
{
  return fRad * (180.0 / M_PI);
}


double DegToRad(double fDeg)
{
  return fDeg * (M_PI / 180.0);
}


bool Equals(double fA, double fB, double fPrecision)
{
    return (fabs(fA - fB) < fPrecision);
}


bool EQPercentageDiff(double fA, double fB, double fPrecision)
{
    return fabs((fA - fB) / ((fA + fB) / 2.0f)) < fPrecision;
}

