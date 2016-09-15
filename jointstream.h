#ifndef CJOINTSTREAM_H
#define CJOINTSTREAM_H

#include "joint.h"
#include "helper.h"

#include <vector>
#include <memory>


class cJointStream : public std::vector<std::shared_ptr<cJoint>>
{
public:
  cJointStream(eJointType eType);

  std::vector<std::shared_ptr<cJointStream>> m_oSubJointStream; // TODO: hast to be private!

  void AddSubJointStream(std::shared_ptr<cJointStream> pJointStream);
  void SetName(string sName);
  void SetOffset(cVector3<float> oOffset);

  std::shared_ptr<cJoint> GetLastJoint();
  std::shared_ptr<cJoint> GetPenultimateJoint();
  cVector3<float> GetOffset();
  float GetLimbLength();
  string GetName();
  eJointType GetType();

private:
  string m_sName;
  const eJointType m_eType;
  cVector3<float> m_oOffset;
};

#endif // CJOINTSTREAM_H
