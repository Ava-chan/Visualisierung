#ifndef CKINECTCSV_H
#define CKINECTCSV_H

#include "csvreader.h"
#include "hierarchicmotion.h"

#include "joint.h"
#include "vector3.hpp"

#include <memory>

#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/graphics.hpp>
#include <fantom/fields.hpp>

class cKinectCSV
{
public:
  virtual ~cKinectCSV() {};

  virtual void LoadFromFile(const string& sFilename);
  std::vector<std::vector<fantom::Point3>> GetJoints();

private:
  cVector3<float> m_oResult;

  std::shared_ptr<cHierarchicMotion> m_pHierarchicMotion;

  void GetJointsFromRow(cCSVRow oRow, std::vector<std::shared_ptr<cJoint>>& oJoints);
};



#endif // CKINECTCSV_H
