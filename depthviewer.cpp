// only include OpenGL if you use a custom drawer. Otherwise,
// there is no need to include any extra header
#include <GL/gl.h>

#include <vector>
#include <utility>

#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/graphics.hpp>
#include <fantom/fields.hpp>
#include <fantom/outputs/VisOutputs.hpp>
using namespace fantom;



#define INPUT_PIN_POSITIONS   "Positions"
#define INPUT_PIN_DEPTHVALUES "Tiefenwerte"
#define OUTPUT_NAME           "Pointcloud"


namespace
{
// inputs
std::vector<double> m_vecX;
std::vector<double> m_vecY;
std::vector<double> m_vecZ;

class cDepthViewer : public VisAlgorithm
{
private:
    // outputs
    std::unique_ptr<Primitive> m_pPointCloud;

    bool InitOutput();
    bool LoadPositions(const Algorithm::Options& options);
    bool LoadDepthValues(const Algorithm::Options& options);


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

        if ((m_vecX.size() == m_vecY.size())
             && (m_vecY.size() == m_vecZ.size())
             && (m_vecX.size() == m_vecZ.size()))
        {
            debugLog() << "Draw Scene" << std::endl;
            m_pPointCloud->addCustom(makePointcloudDrawer);
        }
        else
        {
            debugLog() << "m_vecX: " << m_vecX.size() << std::endl
                       << "m_vecY: " << m_vecY.size() << std::endl
                       << "m_vecZ: " << m_vecZ.size() << std::endl;
        }
    }


    struct GlPointcloudDrawer : public CustomDrawer
    {
        GLuint list;

        GlPointcloudDrawer()
            : list( glGenLists(1))
        {
            glNewList(list, GL_COMPILE );
            glColor3f (1.0, 1.0, 1.0);
            glBegin(GL_POINTS);
            for (int i=0; i<=m_vecX.size(); ++i)
            {
                float fX = -m_vecX[i];
                float fY = m_vecY[i];
                float fZ = -m_vecZ[i];
                glVertex3f(fX, fY, fZ);
            }
            glEnd();
            glEndList();
        }

        ~GlPointcloudDrawer()
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

    static std::unique_ptr<CustomDrawer> makePointcloudDrawer()
    {
        return std::unique_ptr<CustomDrawer>(new GlPointcloudDrawer());
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

AlgorithmRegister<cDepthViewer> dummy("DepthViewer", "Show Depth" );
} // namespace
