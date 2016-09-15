#include "kinectcsv.h"


void cKinectCSV::LoadFromFile(const string& sFilename)
{
  m_pHierarchicMotion = std::make_shared<cHierarchicMotion>(); // TODO: move to constructor

  std::ifstream oFile(sFilename);
  std::vector<std::shared_ptr<cJoint>> oJoints;
  for(cCSVIterator oRow(oFile); oRow != cCSVIterator(); ++oRow) // per line
  {
      GetJointsFromRow((*oRow), oJoints);
      std::int64_t nTime = SToLL((*oRow)[0]);

      if (nTime == 0) continue;

      if (!m_pHierarchicMotion->Initialized())
      {
          m_pHierarchicMotion->Init(oJoints);
      }
      else
      {
          m_pHierarchicMotion->ExtendMotion(nTime, oJoints);
      }
  }
}


void cKinectCSV::GetJointsFromRow(cCSVRow oRow, std::vector<std::shared_ptr<cJoint>>& oJoints)
{
  oJoints.clear();

  eJointType nJointType;
  cVector3<float> oPosition{0, 0, 0};

  // iterate through row and create add joints to list
  for (size_t i=1, counter=0; i<oRow.size()-1; i+=3)
  {
      // type depends on index in row; so we define new type by counting up
      nJointType = static_cast<eJointType>(counter++);
      // position relative to camera
      oPosition[0] = -SToF(oRow[i]);
      oPosition[1] = SToF(oRow[i+1]);
      oPosition[2] = SToF(oRow[i+2]);

      std::shared_ptr<cJoint> pJoint(new cJoint(nJointType, oPosition));
      oJoints.push_back(pJoint);
  }
}


std::vector<std::vector<fantom::Point3>> cKinectCSV::GetJoints()
{
    std::vector<std::vector<fantom::Point3>> vecJointsAsPoints;
    std::vector<std::vector<cJoint>> vecJointJoint = m_pHierarchicMotion->GetJoints();

    for (std::vector<cJoint> vecJoint : vecJointJoint)
    {
        std::vector<fantom::Point3> vecPoints;
        for (cJoint oJoint : vecJoint)
        {
           fantom::Point3 oPoint((-oJoint.GetPosition()[0])+0.05,
                                 (oJoint.GetPosition()[1]) +0.3,
                                 (-oJoint.GetPosition()[2]) +0.1);
           vecPoints.push_back(oPoint);
        }
        vecJointsAsPoints.push_back(vecPoints);
    }
    return vecJointsAsPoints;
}
