#include <GL/gl.h>

#include <vector>
#include <utility>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/graphics.hpp>
#include <fantom/fields.hpp>
#include <fantom/outputs/VisOutputs.hpp>
using namespace fantom;


#define INPUT_PIN_POSITIONS   "Positions"
#define INPUT_PIN_DEPTHVALUES "Tiefenwerte"
#define INPUT_PIN_MINIMA      "Minima"
#define OUTPUT_NAME           "PointCloud"


namespace
{
// inputs, defined static/global to use in PointCloudDrawer
std::vector<double> m_vecX;
std::vector<double> m_vecY;
std::vector<double> m_vecZ;
std::vector<cv::Point> m_minima;

int Minima_left_barrier;
int Minima_right_barrier;
int Minima_top_barrier;
int Minima_bottom_barrier;

class cDepthViewer : public VisAlgorithm
{
private:
    // outputs
    std::unique_ptr<Primitive> m_pPointCloud;

    bool InitOutput();
    bool LoadPositions(const Algorithm::Options& options);
    bool LoadDepthValues(const Algorithm::Options& options);
    bool LoadMinima(const Algorithm::Options& options);


public:
    struct Options :
            public VisAlgorithm::Options
    {
    public:
        Options(Algorithm::Options::Control& control) :
            DataAlgorithm::Options(control)
        {
            add<DomainBase>(INPUT_PIN_POSITIONS, "A point set or grid");
            add<TensorFieldDiscrete<Scalar>>(INPUT_PIN_DEPTHVALUES, "A scattered scalar field");
            add<DomainBase>(INPUT_PIN_MINIMA, "A point set");

            add<int>("Minima_left_barrier", "", 100);
            add<int>("Minima_right_barrier", "", 130);
            add<int>("Minima_top_barrier", "", 20);
            add<int>("Minima_bottom_barrier", "", 50);
        }
    };


    struct VisOutputs :
            public VisAlgorithm::VisOutputs
    {
    public:
        VisOutputs(fantom::VisOutputs::Control &control) :
            VisAlgorithm::VisOutputs(control)
        {
            addGraphics(OUTPUT_NAME);
        }
    };


    cDepthViewer(InitData& data) :
        VisAlgorithm(data)
    {
    }

    void execute(const Algorithm::Options& options, const volatile bool& abortFlag) override
    {
        if (abortFlag)
        {
            return;
        }

        // init triangle output
        InitOutput();

        // read data from input
        if (!LoadPositions(options)) return;
        if (!LoadDepthValues(options)) return;
        LoadMinima(options);


        if ((m_vecX.size() == m_vecY.size())
             && (m_vecY.size() == m_vecZ.size())
             && (m_vecX.size() == m_vecZ.size()))
        {
            debugLog() << "Draw Scene" << std::endl;
            m_pPointCloud->addCustom(makePointCloudDrawer);
        }
        else
        {
            debugLog() << "m_vecX: " << m_vecX.size() << std::endl
                       << "m_vecY: " << m_vecY.size() << std::endl
                       << "m_vecZ: " << m_vecZ.size() << std::endl;
        }
    }

    struct GlPointCloudDrawer : public CustomDrawer
    {
        GLuint list;

        static bool pointsort (cv::Point3d i,cv::Point3d j) { return (i.z<j.z); }


        GlPointCloudDrawer()
            : list( glGenLists(1))
        {
            float failuredistance = 0.1;
            // Calclulate Wall
            float qxyz[12] = {0};
            int qc[4] = {0};
            int width=512, height = 424, ignoreedges = 75;
            for (int w = ignoreedges; w < width-ignoreedges; ++w){
                for (int h = ignoreedges; h < height-ignoreedges; ++h) {
                    int i = w + h * width;
                    if (abs(m_vecZ[i]) > failuredistance) {
                        int add;
                        if (w <width/2 && h < height/2) {
                            add = 0;
                        } else if (w >= width/2 && h < height/2) {
                            add = 1;
                        }else if (w < width/2 && h >= height/2) {
                            add = 2;
                        }else if (w >= width/2 && h >= height/2){
                            add = 3;
                        }
                        qxyz[0+add] -= m_vecX[i];
                        qxyz[4+add] += m_vecY[i];
                        qxyz[8+add] -= m_vecZ[i];
                        qc[0+add]++;
                    }
                }
            }
            for (int i = 0; i < 12; i++) {
                qxyz[i] /= (float)qc[i%4];
            }

            //create plane equation
            cv::Point3d qol = {qxyz[0], qxyz[4],qxyz[8]};
            cv::Point3d qor = {qxyz[1], qxyz[5],qxyz[9]};
            cv::Point3d qul = {qxyz[2], qxyz[6],qxyz[10]};
            cv::Point3d qur = {qxyz[3], qxyz[7],qxyz[11]};

            //crossproduct i,j,k to get the plane normal
     /*     cv::Point3d n = pq.cross(pr);
            float a = (n.x * p.x + n.y * p.y + n.z * p.z) / cv::sqrt(n.x*n.x +n.y*n.y + n.z*n.z);
            float d = (n.x * r.x + n.y * r.y + n.z * r.z - a) / cv::sqrt(n.x*n.x +n.y*n.y + n.z*n.z);*/




            //Draw Wall in points
            glNewList(list, GL_COMPILE );
            glColor3f (1.0, 1.0, 1.0);
            glBegin(GL_POINTS);
            for (int i=0; i<=m_vecX.size(); ++i)
            {
                if (fabs(m_vecZ[i]) > failuredistance) {
                    float fX = -m_vecX[i];
                    float fY = m_vecY[i];
                    float fZ = -m_vecZ[i];
                    glVertex3f(fX, fY, fZ);
                }
            }
            glEnd();

            //Draw Local minima
            for (int j=0; j < m_minima.size(); ++j)
            {
                //Randbereiche ausschließen
                if (m_minima.at(j).x < Minima_left_barrier) continue; // linke schranke
                if (m_minima.at(j).x > 512 - Minima_right_barrier) continue; // rechte schranke
                if (m_minima.at(j).y < Minima_top_barrier) continue; // untere schranke
                if (m_minima.at(j).y > 424 - Minima_bottom_barrier) continue; // obere schranke

                std::vector<cv::Point3d> vec;

                int av = 15;

                for (int y = -10; y <= 10; y ++) {
                    int ai = m_minima.at(j).x + y * 512;
                    float fZr = -m_vecZ[ai-av];
                    float fZl = -m_vecZ[ai+av];

                    float a = (fZl - fZr) /(float)(av+1);
                    if(fZr< 0.1 || fZl < 0.1) a = 0;
                    for (int x = -10; x <= 10; x++) {

                        int rx = m_minima.at(j).x + x;
                        int ry = m_minima.at(j).y + y;

                        int i = (rx) + (ry) * 512;
                        float fX = -m_vecX[i];
                        float fY = m_vecY[i];
                        float fZ = -m_vecZ[i];

                        if (i > 0 && fabs(m_vecZ[i]) > failuredistance) {
                            vec.push_back(cv::Point3d(fX,fY,fZ + ((float)x+10.0)*a*10));
                        }
                    }
                }

                std::sort (vec.begin(), vec.end(), pointsort);

                for (int i = vec.size() - 5; i < vec.size(); i++) {
                    glColor3f ((1.0 / 2.0 * vec.at(i).y), 0.0, 1.0 - (1.0 / 2.0 * vec.at(i).y));
                    glBegin(GL_QUADS);

                    if (i > 0 && fabs(vec.at(i).z) > failuredistance) {
                        glVertex3f(vec.at(i).x+0.03, vec.at(i).y-0.03, vec.at(i).z);
                        glVertex3f(vec.at(i).x+0.03, vec.at(i).y+0.03, vec.at(i).z);
                        glVertex3f(vec.at(i).x-0.03, vec.at(i).y+0.03, vec.at(i).z);
                        glVertex3f(vec.at(i).x-0.03, vec.at(i).y-0.03, vec.at(i).z);
                    }
                    glEnd();
                }
            }

            glColor3f (1.0, 0.0, 0.0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(qxyz[1],qxyz[5],qxyz[9]); //or
            glVertex3f(qxyz[0],qxyz[4],qxyz[8]); //ol
            glVertex3f(qxyz[2],qxyz[6],qxyz[10]); //ul
            glVertex3f(qxyz[3],qxyz[7],qxyz[11]); //ur
            glEnd();

            glEndList();


        }


        ~GlPointCloudDrawer()
        {
            glDeleteLists( list, 1 );
        }

        virtual void draw() const
        {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glCallList(list);
            glPopAttrib();
        }
    };

    static std::unique_ptr<CustomDrawer> makePointCloudDrawer()
    {
        return std::unique_ptr<CustomDrawer>(new GlPointCloudDrawer());
    }
};


bool cDepthViewer::InitOutput()
{
    m_pPointCloud.reset();
    m_pPointCloud = getGraphics(OUTPUT_NAME).makePrimitive();
    return true;
}


bool cDepthViewer::LoadPositions(const Algorithm::Options& options)
{
    infoLog() << "Load Position" << std::endl;
    auto pPositions = options.get<DiscreteDomain<2> >(INPUT_PIN_POSITIONS);
    if (!pPositions)
    {
        debugLog() << "Positions not connected." << std::endl;
        return false;
    }

    m_vecX.clear();
    m_vecY.clear();
    for (size_t i = 0; i < pPositions->numPoints(); ++i)
    {
        m_vecX.push_back(pPositions->points()[i][0]);
        m_vecY.push_back(pPositions->points()[i][1]);
    }
    debugLog() << "VecX: " << m_vecX.size() << " "
               << "VecY: " << m_vecY.size() << std::endl;
    return true;
}


bool cDepthViewer::LoadDepthValues(const Algorithm::Options& options)
{
    infoLog() << "Load DepthValues" << std::endl;
    auto pDepthValues = options.get<TensorFieldDiscrete<Scalar> >(INPUT_PIN_DEPTHVALUES);
    if (!pDepthValues)
    {
        debugLog() << "Tiefenwerte not connected." << std::endl;
        return false;
    }

    m_vecZ.clear();
    auto eval = pDepthValues->makeDiscreteEvaluator();
    for (size_t i=0; i<eval->numValues(); ++i)
    {
        m_vecZ.push_back(eval->value(i)[0]);
    }

    debugLog() << "Tiefenwerte: " << m_vecZ.size() << std::endl;
    return true;
}

bool cDepthViewer::LoadMinima(const Algorithm::Options& options)
{
    infoLog() << "Load DepthValues" << std::endl;
    auto minValues = options.get<DiscreteDomain<2>>(INPUT_PIN_MINIMA);
    if (!minValues)
    {
        debugLog() << "Minima not connected." << std::endl;
        return false;
    }

    Minima_left_barrier = options.get<int>("Minima_left_barrier");
    Minima_right_barrier = options.get<int>("Minima_right_barrier");
    Minima_top_barrier = options.get<int>("Minima_top_barrier");
    Minima_bottom_barrier = options.get<int>("Minima_bottom_barrier");

    m_minima.clear();
    for (size_t i = 0; i < minValues->numPoints(); ++i)
    {
        m_minima.push_back(cv::Point(minValues->points()[i][0], minValues->points()[i][1]));
    }

    debugLog() << "Minima: " << m_minima.size() << std::endl;
    return true;
}

AlgorithmRegister<cDepthViewer> dummy("DepthViewer", "Show Depth");
} // namespace
