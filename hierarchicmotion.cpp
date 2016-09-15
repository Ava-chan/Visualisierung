#include "hierarchicmotion.h"


cHierarchicMotion::cHierarchicMotion() :
    m_bInit{false},
    m_nInitCallCounter{0}
{
    for (int i=0; i<JT_Count; ++i)
    {
        auto eType = static_cast<eJointType>(i);
        m_mapJointStreams[eType] = std::shared_ptr<cJointStream>(new cJointStream{eType});
    }

    m_vecTimeStream = std::make_shared<std::vector<std::int64_t>>();

    BuildHierarchy();
}


void cHierarchicMotion::BuildHierarchy()
{
// use spine_mid as root

// lower body
m_mapJointStreams[JT_SpineBase]->AddSubJointStream(m_mapJointStreams[JT_SpineMid]);
// left hip
m_mapJointStreams[JT_SpineBase]->AddSubJointStream(m_mapJointStreams[JT_HipLeft]);
// left leg
m_mapJointStreams[JT_HipLeft]->AddSubJointStream(m_mapJointStreams[JT_KneeLeft]);
m_mapJointStreams[JT_KneeLeft]->AddSubJointStream(m_mapJointStreams[JT_AnkleLeft]);

// left food
m_mapJointStreams[JT_AnkleLeft]->AddSubJointStream(m_mapJointStreams[JT_FootLeft]);
// right hip
m_mapJointStreams[JT_SpineBase]->AddSubJointStream(m_mapJointStreams[JT_HipRight]);
// left leg
m_mapJointStreams[JT_HipRight]->AddSubJointStream(m_mapJointStreams[JT_KneeRight]);
m_mapJointStreams[JT_KneeRight]->AddSubJointStream(m_mapJointStreams[JT_AnkleRight]);
// left food
m_mapJointStreams[JT_AnkleRight]->AddSubJointStream(m_mapJointStreams[JT_FootRight]);
// upper body
m_mapJointStreams[JT_SpineMid]->AddSubJointStream(m_mapJointStreams[JT_SpineShoulder]);
m_mapJointStreams[JT_SpineShoulder]->AddSubJointStream(m_mapJointStreams[JT_Neck]);
m_mapJointStreams[JT_Neck]->AddSubJointStream(m_mapJointStreams[JT_Head]);
// left shoulder
m_mapJointStreams[JT_SpineShoulder]->AddSubJointStream(m_mapJointStreams[JT_ShoulderLeft]);
// left arm
m_mapJointStreams[JT_ShoulderLeft]->AddSubJointStream(m_mapJointStreams[JT_ElbowLeft]);
m_mapJointStreams[JT_ElbowLeft]->AddSubJointStream(m_mapJointStreams[JT_WristLeft]);
m_mapJointStreams[JT_WristLeft]->AddSubJointStream(m_mapJointStreams[JT_HandLeft]);
// left hand
m_mapJointStreams[JT_HandLeft]->AddSubJointStream(m_mapJointStreams[JT_ThumbLeft]);
m_mapJointStreams[JT_HandLeft]->AddSubJointStream(m_mapJointStreams[JT_HandTipLeft]);

// right shoulder
m_mapJointStreams[JT_SpineShoulder]->AddSubJointStream(m_mapJointStreams[JT_ShoulderRight]);
// right arm
m_mapJointStreams[JT_ShoulderRight]->AddSubJointStream(m_mapJointStreams[JT_ElbowRight]);
m_mapJointStreams[JT_ElbowRight]->AddSubJointStream(m_mapJointStreams[JT_WristRight]);
m_mapJointStreams[JT_WristRight]->AddSubJointStream(m_mapJointStreams[JT_HandRight]);

// right hand
m_mapJointStreams[JT_HandRight]->AddSubJointStream(m_mapJointStreams[JT_ThumbRight]);
m_mapJointStreams[JT_HandRight]->AddSubJointStream(m_mapJointStreams[JT_HandTipRight]);
}


bool CompareOffsets(std::shared_ptr<cJoint> oA, std::shared_ptr<cJoint> oB)
{
    return (oA->GetOffset().Magnitude() < oB->GetOffset().Magnitude());
}


void cHierarchicMotion::Init(std::vector<std::shared_ptr<cJoint>> oJoints)
{
    // validity check
    if (Zero(oJoints))
    {
        return;
    }

    AddJointSet(oJoints);

    // save offset for last elements in stream at
    for (int i=0; i<JT_Count; ++i)
    {
        auto eType = static_cast<eJointType>(i);
        auto oPosition = m_mapJointStreams[eType]->GetLastJoint()->GetPosition();

        for (auto pSubJointStream : m_mapJointStreams[eType]->m_oSubJointStream)
        {
            auto oSubPosition = pSubJointStream->GetLastJoint()->GetPosition();
            auto oOffset = oSubPosition - oPosition;

            pSubJointStream->GetLastJoint()->SetOffset(oOffset);
        }
    }

    // and create median of sizes of the individual limbs
    if (++m_nInitCallCounter <= nNumberOfInitSets)
    {
        return;
    }


    // sort joints of different streams
    for (auto oMapElements : m_mapJointStreams)
    {
        auto pJointStream = oMapElements.second;
        std::sort(pJointStream->begin(), pJointStream->end(), CompareOffsets);

        // create median for offset
        std::shared_ptr<cJoint> pExampleJoint = pJointStream->at(pJointStream->size() / 2);
        pJointStream->SetOffset(pExampleJoint->GetOffset());
        // clear jointstream and make sure there is one joint in the stream
        pJointStream->clear();
        pJointStream->push_back(pExampleJoint);
    }

    // Print result of init to screen
    std::cout << "------------- INIT --------------" << std::endl;
    for (auto oMapElements : m_mapJointStreams)
    {
        auto pStream = oMapElements.second;
        std::cout << pStream->GetName() << " = " << pStream->GetOffset().Magnitude() << std::endl;
    }
    std::cout << "------------- DONE --------------" << std::endl;

    m_bInit = true;
}




void cHierarchicMotion::AddJointSet(std::vector<std::shared_ptr<cJoint>> oJoints)
{
    for (auto pJoint : oJoints)
    {
        m_mapJointStreams[pJoint->GetType()]->push_back(pJoint);
    }
}


void cHierarchicMotion::RemoveLastJointSet()
{
    for (int i=0; i<JT_Count; ++i)
    {
        auto eType = static_cast<eJointType>(i);
        m_mapJointStreams[eType]->pop_back();
    }
}


bool cHierarchicMotion::Zero(std::vector<std::shared_ptr<cJoint>>& oJoints)
{
    float fMaxX = -100, fMinX = 100, fMaxY = -100, fMinY = 100, fMaxZ = -100, fMinZ = 100;

    float fCurX, fCurY, fCurZ;

    for (auto pJoint : oJoints)
    {
        fCurX = static_cast<float>(pJoint->GetPosition()[0]);
        fCurY = static_cast<float>(pJoint->GetPosition()[1]);
        fCurZ = static_cast<float>(pJoint->GetPosition()[2]);

        fMinX = (fCurX < fMinX) ? fCurX : fMinX;
        fMinY = (fCurY < fMinY) ? fCurY : fMinY;
        fMinZ = (fCurZ < fMinZ) ? fCurZ : fMinZ;

        fMaxX = (fCurX > fMaxX) ? fCurX: fMaxX;
        fMaxY = (fCurY > fMaxY) ? fCurY : fMaxY;
        fMaxZ = (fCurZ > fMaxZ) ? fCurZ : fMaxZ;
    }

    float fXSize = fabs(fMaxX-fMinX);
    float fYSize = fabs(fMaxY-fMinY);
    float fZSize = fabs(fMaxZ-fMinZ);

    return ((fXSize < 0.1f) && (fYSize<0.01f) && (fZSize < 0.1f));
}


void cHierarchicMotion::ExtendMotion(std::int64_t nTime, std::vector<std::shared_ptr<cJoint>> oJoints)
{
    // validity check
    if (Zero(oJoints))
    {
        return;
    }

    // add joints to data structure, this is necessary for a validity check
    AddJointSet(oJoints);

    // Check currently meassured limbs have right length
    for (int i=0; i<JT_Count; ++i)
    {
        auto eType = static_cast<eJointType>(i);
        auto pCurrentJointStream = m_mapJointStreams[eType];

        auto pCurJoint = pCurrentJointStream->GetLastJoint();

        auto oCurJointPos  = pCurJoint->GetPosition();

        for (auto pSubJointStream : pCurrentJointStream->m_oSubJointStream)
        {
            auto pSubJoint = pSubJointStream->GetLastJoint();
            auto oSubJointPos  = pSubJoint->GetPosition();

            auto oOffset = oCurJointPos-oSubJointPos;
            auto fCurLimbLength = oOffset.Magnitude();
            auto fLimbLength = pSubJointStream->GetLimbLength();

            pSubJoint->SetOffset(oCurJointPos-oSubJointPos);

            if (!EQPercentageDiff(static_cast<double>(fLimbLength),
                                  static_cast<double>(fCurLimbLength), 0.3f)
                && pSubJointStream->GetType() != JT_HandLeft
                && pSubJointStream->GetType() != JT_HandTipLeft
                && pSubJointStream->GetType() != JT_HandRight
                && pSubJointStream->GetType() != JT_HandTipRight
                && pSubJointStream->GetType() != JT_ThumbLeft
                && pSubJointStream->GetType() != JT_ThumbRight
                && pSubJointStream->GetType() != JT_FootLeft
                && pSubJointStream->GetType() != JT_FootRight
                && pSubJointStream->GetType() != JT_Head)
            {

                std::cout << pSubJointStream->GetName()
                          << " (" << fabs(fLimbLength - fCurLimbLength) << ")"
                          << " dont fit" << std::endl;

                RemoveLastJointSet();
                return;
            }
        }
    }

    std::cout << "Es passt mal was!!!!!! ... quasi" << std::endl;
}


bool cHierarchicMotion::Initialized()
{
    return m_bInit;
}


std::vector<std::vector<cJoint>> cHierarchicMotion::GetJoints()
{
    std::vector<std::vector<cJoint>> vecOfJointVecs;
    for (int i=0; i<JT_Count; ++i)
    {
        std::vector<cJoint> vecJoint;
        auto eType = static_cast<eJointType>(i);
        for (std::shared_ptr<cJoint> pJoint : *m_mapJointStreams[eType])
        {
            vecJoint.push_back(*pJoint);
        }
        vecOfJointVecs.push_back(vecJoint);
    }
    return vecOfJointVecs;
}
