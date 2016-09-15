#ifndef VECTOR3_H
#define VECTOR3_H

#include "helper.h"

#include <string>
#include <sstream>
#include <math.h>
#include <memory>
#include <iomanip>


template<class T>
class cVector3
{
public:
    cVector3() :
        m_fTuple{}
      {
      }

    cVector3(T x, T y, T z) :
        m_fTuple{x, y, z}
      {
      }

    // ‘this’ is vector U
    cVector3 operator+(const cVector3& oV) const; //U+V
    cVector3 operator-(const cVector3& oV) const; //U-V
    cVector3& operator*=(T nS); //U-V

    T Magnitude() const;

    T operator[](int i) const;
    T &operator[](int i);

    void ToNorm();
    cVector3<float> Cross(const cVector3<float>& oV) const;

    std::string ToString();

private:
    T m_fTuple[3];
};

// float CalculateDistance(const cVector3<float>& a, const cVector3<float>& b);


template<class T>
T cVector3<T>::operator[](int i) const
{
    return m_fTuple[i];
}


template<class T>
T &cVector3<T>::operator[](int i)
{
    return m_fTuple[i];
}


template<class T>
cVector3<T> cVector3<T>::operator+(const cVector3<T>& oV) const
{
  return cVector3f(m_fTuple[0] + oV.m_fTuple[0],
      m_fTuple[1] + oV.m_fTuple[1],
      m_fTuple[2] + oV.m_fTuple[2]);
}


template<class T>
cVector3<T> cVector3<T>::operator-(const cVector3<T>& oV) const
{
  return cVector3<T>(m_fTuple[0] - oV.m_fTuple[0],
                 m_fTuple[1] - oV.m_fTuple[1],
                 m_fTuple[2] - oV.m_fTuple[2]);
}


template<class T>
cVector3<T>& cVector3<T>::operator*=(T nS)
{
    m_fTuple[0] *= nS;
    m_fTuple[1] *= nS;
    m_fTuple[2] *= nS;
    return (*this);
}


template<class T>
T cVector3<T>::Magnitude() const
{
  return sqrt(m_fTuple[0] * m_fTuple[0]
      + m_fTuple[1] * m_fTuple[1]
      + m_fTuple[2] * m_fTuple[2]);
}


template<class T>
void cVector3<T>::ToNorm()
{
  float fLen = Magnitude();
  m_fTuple[0] = (1.0f/fLen) * m_fTuple[0];
  m_fTuple[1] = (1.0f/fLen) * m_fTuple[1];
  m_fTuple[2] = (1.0f/fLen) * m_fTuple[2];
}


template<class T>
cVector3<float> cVector3<T>::Cross(const cVector3<float>& oV) const
{
  cVector3<float> oCrossProduct;
  oCrossProduct[0] = (*this)[1] * oV[2] - oV[1] * (*this)[2];
  oCrossProduct[1] = (*this)[2] * oV[0] - oV[2] * (*this)[0];
  oCrossProduct[2] = (*this)[0] * oV[1] - oV[0] * (*this)[1];

  return oCrossProduct;
}


template<class T>
std::string cVector3<T>::ToString()
{
    std::stringstream ss;
    ss.str(std::string());
    ss.clear();
    ss << "["
       << std::setprecision(3)
       << std::setw(5)
       << m_fTuple[0]
       << ", " << m_fTuple[1]
       << ", " << m_fTuple[2]
       << "]";
    return ss.str();
}

#endif // VECTOR3_H
