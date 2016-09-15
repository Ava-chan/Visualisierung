#include "joint.h"


cJoint::cJoint(eJointType eJointType, cVector3<float> oPosition) :
    m_eJointType(eJointType),
    m_oPosition(oPosition)
{
}


cVector3<float>& cJoint::GetPosition()
{
    return m_oPosition;
}


eJointType cJoint::GetType()
{
    return m_eJointType;
}


void cJoint::SetOffset(cVector3<float> oOffset)
{
    m_oOffset = oOffset;
}


cVector3<float> cJoint::GetOffset()
{
    return m_oOffset;
}

