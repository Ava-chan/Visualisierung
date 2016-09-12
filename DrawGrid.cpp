#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/graphics.hpp>
#include <fantom/fields.hpp>
using namespace fantom;

#include "VisHelper.h"
#include "Delaunay.h"

#define MAXPOINTS 512

namespace
{

class DrawGridAlgorithm : public VisAlgorithm
{
    // Zeiger auf interne Grafikobjekte
    std::unique_ptr< Primitive > mTriangle;


    del_point2d_t mDelaunayPoints[MAXPOINTS];

public:
    // Ausgaben der Visualisierung bekanntmachen
    struct VisOutputs : public VisAlgorithm::VisOutputs
    {
        VisOutputs(fantom::VisOutputs::Control& control) : VisAlgorithm::VisOutputs(control)
        {
            addGraphics( "triangle" );
        }
    };


    struct Options : public DataAlgorithm::Options
    {
    public:
        Options(Algorithm::Options::Control& control) : DataAlgorithm::Options(control)
        {
            // Die Datenpunkte
            add<All_DiscreteDomain>("Domain", "");

            // Optional ein Feld mit Werten
            add<TensorFieldDiscrete<Scalar> >("Field", "");

            // Zusätzliche Optionen, wie Farbe und Goesse
            add<Color>("Color", "", Color(0,1,0));
            add<double>("SizeFactor", "", 1.0);
            add<double>("Distance", "", 4);

        }
    };


    DrawGridAlgorithm(InitData& data) : VisAlgorithm(data)
    {
    }


    virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
    {
        mTriangle.reset();
        mTriangle = getGraphics("triangle").makePrimitive();

        try
        {
            auto domain = options.get<DiscreteDomain<2> >("Domain");
            auto color = options.get<Color>("Color");
            auto sizeFactor = options.get<double>("SizeFactor");

            auto field = options.get<const TensorFieldDiscrete<Scalar> >("Field");

            if (!domain)
            {
                debugLog() << "Domain not connected." << std::endl;
                return;
            }

            for (size_t i = 0; i < domain->numPoints(); ++i)
            {
                mDelaunayPoints[i].x = static_cast<real>(domain->points()[i][0]);
                mDelaunayPoints[i].y = static_cast<real>(domain->points()[i][1]);

                infoLog() << "Point " << i << " (" << mDelaunayPoints[i].x << ", " << mDelaunayPoints[i].y << ")\n";
            }

            delaunay2d_t* res = delaunay2d_from(mDelaunayPoints, domain->numPoints());
            tri_delaunay2d_t*	tdel	= tri_delaunay2d_from(res);

            // Testen welche Eingaben gesetzt sind
            if (field)
            {
                infoLog() << "Zeige Werte und Punkte an." << std::endl;
                auto eval = field->makeDiscreteEvaluator();

                std::map<std::pair<double, double>, double> pointValueMap;

                for (size_t i = 0; i < domain->numPoints(); ++i)
                {
                    pointValueMap[std::pair<double, double>(domain->points()[i][0], domain->points()[i][1])] = eval->value(i)[0];
                }

                double leastValue = 100.0;
                double highestValue = -100.0;

                for (size_t i = 0; i < domain->numPoints(); ++i)
                {
                    double value = eval->value(i)[0];
                    if (value < leastValue)
                    {
                        leastValue = value;
                    }
                    if (value > highestValue)
                    {
                        highestValue = value;
                    }
                }

                double distanceHighestLeast = highestValue - leastValue;

                for (size_t i = 0; i < domain->numPoints(); ++i)
                {
                    auto value = eval->value(i)[0];
                    double currentValueInScale = value - leastValue;

                        double f = (value - leastValue) / distanceHighestLeast;
                        color = Color(1.0f, f, 0.0f, 1.0f);

                        /*
                        mPoints->addSphere(toPoint3(domain->points()[i]),
                                           (currentValueInScale / distanceHighestLeast * size * 2.0f) + size/3.0f,
                                           color);
                        */
                        // mLabels->addTextLabel(toPoint3(domain->points()[i]), std::to_string(value), 24);

                        for(size_t i = 0; i < tdel->num_triangles; i++ )
                        {
                            std::vector< Point3 > tri( 3 );
                            tri[0] = Point3( tdel->points[tdel->tris[i * 3]].x, tdel->points[tdel->tris[i * 3]].y,
                                    pointValueMap[std::pair<double, double>(tdel->points[tdel->tris[i * 3]].x, tdel->points[tdel->tris[i * 3]].y)] - leastValue / distanceHighestLeast * sizeFactor);
                            tri[1] = Point3( tdel->points[tdel->tris[i * 3 + 1]].x, tdel->points[tdel->tris[i * 3 + 1]].y,
                                    pointValueMap[std::pair<double, double>(tdel->points[tdel->tris[i * 3 + 1]].x, tdel->points[tdel->tris[i * 3 + 1]].y)] - leastValue / distanceHighestLeast * sizeFactor);
                            tri[2] = Point3( tdel->points[tdel->tris[i * 3 + 2]].x, tdel->points[tdel->tris[i * 3 + 2]].y,
                                    pointValueMap[std::pair<double, double>(tdel->points[tdel->tris[i * 3 + 2]].x, tdel->points[tdel->tris[i * 3 + 2]].y)] - leastValue / distanceHighestLeast * sizeFactor);
                            mTriangle->add( Primitive::LINE_LOOP )
                                    .setLineWidth(1.0)
                                    .setColor(color)
                                    .setVertices(tri);
                        }

                }
            }
            else
            {
                for(size_t i = 0; i < tdel->num_triangles; i++ )
                {
                    std::vector< Point3 > tri( 3 );
                    tri[0] = Point3( tdel->points[tdel->tris[i * 3]].x, tdel->points[tdel->tris[i * 3]].y, 0.0 );
                    tri[1] = Point3( tdel->points[tdel->tris[i * 3 + 1]].x, tdel->points[tdel->tris[i * 3 + 1]].y, 0.0 );
                    tri[2] = Point3( tdel->points[tdel->tris[i * 3 + 2]].x, tdel->points[tdel->tris[i * 3 + 2]].y, 0.0 );
                    mTriangle->add( Primitive::LINE_LOOP )
                            .setLineWidth(1.0)
                            .setColor(color)
                            .setVertices(tri);
                }
            }
            tri_delaunay2d_release(tdel);
            delaunay2d_release(res);
        }
        catch (fantom::logic_error err) // catch exception, if we do not receive right types from input
        {
            debugLog() << err.getBacktrace() << std::endl;
            return;
        }
    }
};

AlgorithmRegister<DrawGridAlgorithm> dummy("Grid/DrawGrid", "Show some example graphics.");
} // namespace
