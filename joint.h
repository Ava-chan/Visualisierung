#ifndef CJOINT_H
#define CJOINT_H


#include "vector3.hpp"


#include <vector>
#include <string>
#include <utility>
#include <memory>
#include <algorithm>
using namespace std;


enum eJointType
{
  JT_SpineBase = 0,
  JT_SpineMid = 1,
  JT_Neck = 2,
  JT_Head = 3,
  JT_ShoulderLeft = 4,
  JT_ElbowLeft = 5,
  JT_WristLeft = 6,
  JT_HandLeft = 7,
  JT_ShoulderRight = 8,
  JT_ElbowRight = 9,
  JT_WristRight = 10,
  JT_HandRight = 11,
  JT_HipLeft = 12,
  JT_KneeLeft = 13,
  JT_AnkleLeft = 14,
  JT_FootLeft = 15,
  JT_HipRight = 16,
  JT_KneeRight = 17,
  JT_AnkleRight = 18,
  JT_FootRight = 19,
  JT_SpineShoulder = 20,
  JT_HandTipLeft = 21,
  JT_ThumbLeft = 22,
  JT_HandTipRight = 23,
  JT_ThumbRight = 24,
  JT_Count = (JT_ThumbRight + 1)
};


class cJoint
{
public:
  cJoint(eJointType eJointType, cVector3<float> oPosition);

  cVector3<float>& GetPosition();
  cVector3<float> GetOffset();
  std::string GetTypeName();
  eJointType GetType();

  void SetOffset(cVector3<float> oOffset);

  std::string ToString();

private:
  bool m_bTracked;
  eJointType m_eJointType;

  cVector3<float> m_oPosition;
  cVector3<float> m_oOffset;

  std::vector<std::shared_ptr<cJoint>> m_oSubJoints;

  std::string JointTypeAsString(eJointType nType);
};

#endif // CJOINT_H
