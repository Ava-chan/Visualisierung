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
#define OUTPUT_NAME           "TriangleStrip"


namespace
{

class cDepthViewer : public VisAlgorithm
{
private:
    // inputs
    std::vector<Point2> m_vecPositions;
    std::vector<double> m_vecDepthValues;

    // outputs
    std::unique_ptr<Primitive> m_pTriangle;


    void InitOutput();
    void LoadPositions(const Algorithm::Options& options);
    void LoadDepthValues(const Algorithm::Options& options);
    void DrawScene();


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
        LoadPositions(options);
        LoadDepthValues(options);

        DrawScene();
    }
};


void cDepthViewer::InitOutput()
{
    m_pTriangle.reset();
    m_pTriangle = getGraphics(OUTPUT_NAME).makePrimitive();
}


void cDepthViewer::LoadPositions(const Algorithm::Options& options)
{
    infoLog() << "Get Position" << std::endl;
    auto pPositions = options.get<DiscreteDomain<2> >(INPUT_PIN_POSITIONS);
    if (!pPositions)
    {
        debugLog() << "Positions not connected." << std::endl;
        return;
    }

    for (size_t i = 0; i < pPositions->numPoints(); ++i)
    {
        m_vecPositions.push_back(pPositions->points()[i]);
    }
}


void cDepthViewer::LoadDepthValues(const Algorithm::Options& options)
{
    infoLog() << "Get DepthValues" << std::endl;
    auto pDepthValues = options.get<TensorFieldDiscrete<Scalar> >(INPUT_PIN_DEPTHVALUES);
    if (!pDepthValues)
    {
        debugLog() << "Tiefenwerte not connected." << std::endl;
        return;
    }

    auto eval = pDepthValues->makeDiscreteEvaluator();
    for (size_t i=0; i<eval->numValues(); ++i)
    {
        m_vecDepthValues.push_back(eval->value(i)[0]);
    }
}


void cDepthViewer::DrawScene()
{
    // simple example ...
    std::vector<Color> vecColorTriangleStrip(3);
    vecColorTriangleStrip.push_back(Color(1.0, 1.0, 1.0, 1.0));
    vecColorTriangleStrip.push_back(Color(1.0, 1.0, 1.0, 1.0));
    vecColorTriangleStrip.push_back(Color(1.0, 1.0, 1.0, 1.0));

    std::vector<Point3> vecTriangleStrip(3);
    vecTriangleStrip.push_back(Point3(-0.75f, 0.0f, 0.0f));
    vecTriangleStrip.push_back(Point3( 0.00f, 1.0f, 0.0f));
    vecTriangleStrip.push_back(Point3( 0.75f, 0.0f, 0.0f));

    m_pTriangle->add(Primitive::TRIANGLE_STRIP)
            .setLineWidth(1.0)
            .setColors(vecColorTriangleStrip)
            .setVertices(vecTriangleStrip);
}

AlgorithmRegister<cDepthViewer> dummy("DepthViewer", "Show Depth" );
} // namespace
