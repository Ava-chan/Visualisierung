#include "kinectcsv.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/graphics.hpp>
#include <fantom/fields.hpp>
using namespace fantom;

#define OUTPIN_TIME "TIME"

#define OUTPIN_SPINBASE "SpineBase"
#define OUTPIN_SPINMID "SpineMid"
#define OUTPIN_NEC "Neck"
#define OUTPIN_HEAD "Head"
#define OUTPIN_SHOULDERLEFT "ShoulderLeft"
#define OUTPIN_ELBOWLEFT "ElbowLeft"
#define OUTPIN_WRISTLEFT "WristLeft"
#define OUTPIN_HANDLEFT "HandLeft"
#define OUTPIN_SHOULDERRIGHT "ShoulderRight"
#define OUTPIN_ELBOWRIGHT "ElbowRight"
#define OUTPIN_WRISTRIGHT "WristRight"
#define OUTPIN_HANDRIGHT "HandRight"
#define OUTPIN_HIPLEFT "HipLeft"
#define OUTPIN_KNEELEFT "KneeLeft"
#define OUTPIN_ANKLELEFT "AnkleLeft"
#define OUTPIN_FOOTLEFT "FootLeft"
#define OUTPIN_HIPRIGHT "HipRight"
#define OUTPIN_KNEERIGHT "KneeRight"
#define OUTPIN_ANKLERIGHT "AnkleRight"
#define OUTPIN_FOOTRIGHT "FootRight"
#define OUTPIN_SPINESHOULDER "SpineShoulder"
#define OUTPIN_HANDTIPLEFT "HandTipLeft"
#define OUTPIN_THUMBLEFT "ThumbLeft"
#define OUTPIN_HANDTIPRIGTH "HandTipRight"
#define OUTPIN_THUMBRIGTH "ThumbRight"


std::vector<std::string> m_vecJointNames = {
    OUTPIN_SPINBASE,
    OUTPIN_SPINMID,
    OUTPIN_NEC,
    OUTPIN_HEAD,
    OUTPIN_SHOULDERLEFT,
    OUTPIN_ELBOWLEFT,
    OUTPIN_WRISTLEFT,
    OUTPIN_HANDLEFT,
    OUTPIN_SHOULDERRIGHT,
    OUTPIN_ELBOWRIGHT,
    OUTPIN_WRISTRIGHT,
    OUTPIN_HANDRIGHT,
    OUTPIN_HIPLEFT,
    OUTPIN_KNEELEFT,
    OUTPIN_ANKLELEFT,
    OUTPIN_FOOTLEFT,
    OUTPIN_HIPRIGHT,
    OUTPIN_KNEERIGHT,
    OUTPIN_ANKLERIGHT,
    OUTPIN_FOOTRIGHT,
    OUTPIN_SPINESHOULDER,
    OUTPIN_HANDTIPLEFT,
    OUTPIN_THUMBLEFT,
    OUTPIN_HANDTIPRIGTH,
    OUTPIN_THUMBRIGTH,
};


namespace
{

    class cMotionLoader : public VisAlgorithm
    {
        std::vector<std::unique_ptr<Primitive>> m_vecJoints;
        std::vector<std::vector<Point3>> m_vecJointPositions;
        std::vector<std::vector<Color>> m_vecJointColors;

    public:

        struct Options : public DataAlgorithm::Options
        {
            Options(Algorithm::Options::Control& control) :
                DataAlgorithm::Options(control)
            {
                add<InputLoadPath>("Input File", "The file to be read", "");
            }
        };


        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            VisOutputs(fantom::VisOutputs::Control& control) : VisAlgorithm::VisOutputs(control)
            {
                for (auto sJoint : m_vecJointNames)
                {
                    addGraphics(sJoint);
                }
            }
        };


        cMotionLoader(InitData& data) : VisAlgorithm(data)
        {
        }


        void LoadTestData()
        {
            m_vecJointPositions.clear();
            m_vecJointColors.clear();
            for (int i=0; i<m_vecJoints.size(); ++i)
            {
                std::vector<Point3> vecPosition;
                std::vector<Color> vecColor;

                for (int j=0; j<10; ++j)
                {
                    vecPosition.push_back(Point3(static_cast<float>(i),
                                                 static_cast<float>(j),
                                                 0.0f));
                    vecColor.push_back(Color(static_cast<float>(j) * (1.0f / 10.0),
                                             1.0f, 1.0f));
                }

                m_vecJointPositions.push_back(vecPosition);
                m_vecJointColors.push_back(vecColor);
            }
        }

        /* http://andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients */
        fantom::Color GetHeatMapColor(float fValue)
        {
          const int NUM_COLORS = 4;
          static float color[NUM_COLORS][3] = { {0,0,1}, {0,1,0}, {1,1,0}, {1,0,0} };
            // A static array of 4 colors:  (blue,   green,  yellow,  red) using {r,g,b} for each.

          int nIdx1;        // |-- Our desired color will be between these two indexes in "color".
          int nIdx2;        // |
          float fFractBetween = 0;  // Fraction between "idx1" and "idx2" where our value is.

          if(fValue <= 0)
          {
              nIdx1 = nIdx2 = 0;
          }    // accounts for an input <=0
          else if(fValue >= 1)
          {
              nIdx1 = nIdx2 = NUM_COLORS-1;
          }    // accounts for an input >=0
          else
          {
            fValue = fValue * (NUM_COLORS-1);        // Will multiply value by 3.
            nIdx1  = floor(fValue);                  // Our desired color will be after this index.
            nIdx2  = nIdx1+1;                        // ... and before this index (inclusive).
            fFractBetween = fValue - float(nIdx1);    // Distance between the two indexes (0-1).
          }

          return fantom::Color((color[nIdx2][0] - color[nIdx1][0])*fFractBetween + color[nIdx1][0],
                               (color[nIdx2][1] - color[nIdx1][1])*fFractBetween + color[nIdx1][1],
                               (color[nIdx2][2] - color[nIdx1][2])*fFractBetween + color[nIdx1][2]);
        }


        void execute(const Algorithm::Options& parameters, const volatile bool& abortFlag) override
        {
            if (abortFlag)
            {
                return;
            }

            if (parameters.get<std::string>("Input File") != "")
            {
                std::string sFilename = parameters.get<std::string>("Input File");

                m_vecJoints.clear();
                for (auto sJoint : m_vecJointNames)
                {
                    m_vecJoints.push_back(getGraphics(sJoint).makePrimitive());
                }

                cKinectCSV oKinect;
                oKinect.LoadFromFile(sFilename);
                std::vector<std::vector<fantom::Point3>> vecVecPoint3 = oKinect.GetJoints();

                m_vecJointPositions.clear();
                for (int i=0; i<m_vecJoints.size(); ++i)
                {
                    m_vecJointPositions.push_back(vecVecPoint3[i]);

                }

                for (int i=0; i<m_vecJoints.size(); ++i)
                {
                    m_vecJoints[i]->add(Primitive::LINE_STRIP).setLineWidth(5.0)
                                                              .setColor(GetHeatMapColor((1.0f / m_vecJoints.size()) * static_cast<float>(i)))
                                                              .setVertices(m_vecJointPositions[i]);
                }
            }
        }

        IMPLEMENT_CAN_HANDLE(".csv")

        static bool loaderSetOptions(Algorithm::Options& options, std::vector<std::string>& filenames)
        {
            // find the name we will handle
            for (auto it = filenames.begin(); it != filenames.end(); ++it)
            {
                if (it->substr(it->size()-4, 4) == ".csv")
                {
                    options.set<std::string>("Input File", *it);
                    it = filenames.erase(it);
                    return true;
                }
            }
            return false;
        }
    };

    AlgorithmRegister<cMotionLoader> dummy("Motionloader", "Load and view motion");
} // namespace
