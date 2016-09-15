#include "jointstream.h"


cJointStream::cJointStream(eJointType eType) :
  m_eType(eType)
{
    m_oOffset[0] = 0;
    m_oOffset[1] = 0;
    m_oOffset[2] = 0;
}


void cJointStream::AddSubJointStream(std::shared_ptr<cJointStream> pJointStream)
{
  m_oSubJointStream.push_back(pJointStream);
}


void cJointStream::SetOffset(cVector3<float> oOffset)
{
  if (Equals(static_cast<double>(m_oOffset.Magnitude()), 0, 0.0001))
  {
    m_oOffset = oOffset;
  }
  else
  {
    float fMeassuredLength = oOffset.Magnitude();

    float fCurLength = m_oOffset.Magnitude();
    m_oOffset.ToNorm();

    float fNewLength = (fMeassuredLength + fCurLength) / 2;
    m_oOffset *= fNewLength;
  }
}


cVector3<float> cJointStream::GetOffset()
{
  return m_oOffset;
}


float cJointStream::GetLimbLength(void)
{
  return m_oOffset.Magnitude();
}


std::shared_ptr<cJoint> cJointStream::GetLastJoint()
{
    return this->at(this->size()-1);
}


std::shared_ptr<cJoint> cJointStream::GetPenultimateJoint()
{
    return this->at(this->size()-2);
}


void cJointStream::SetName(string sName)
{
    m_sName = sName;
}

string cJointStream::GetName()
{
    return m_sName;
}


eJointType cJointStream::GetType()
{
    return m_eType;
}

