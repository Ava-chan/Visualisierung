#ifndef CHIERARCHICMOTION_H
#define CHIERARCHICMOTION_H

#include "jointstream.h"

#include "joint.h"
#include "helper.h"

#include <map>
#include <vector>
#include <iostream>


class cHierarchicMotion
{
public:
  cHierarchicMotion();

  void Init(std::vector<std::shared_ptr<cJoint>> pJoints);
  void ExtendMotion(std::int64_t nTime, std::vector<std::shared_ptr<cJoint>> oJoints);

  std::vector<std::vector<cJoint>> GetJoints();

  std::int64_t GetTime(unsigned long nId);

  unsigned long Size();
  bool Initialized();


private:
  bool m_bInit;
  const unsigned nNumberOfInitSets = 50;
  unsigned m_nInitCallCounter;
  cVector3<float> m_oPosition;

  std::map<eJointType, std::shared_ptr<cJointStream>> m_mapJointStreams;
  std::shared_ptr<std::vector<std::int64_t>> m_vecTimeStream;

  void AddJointSet(std::vector<std::shared_ptr<cJoint>> vecJoints);
  void RemoveLastJointSet();

  void BuildHierarchy();

  bool Zero(std::vector<std::shared_ptr<cJoint>>& oJoints);
};

#endif // CHIERARCHICMOTION_H
