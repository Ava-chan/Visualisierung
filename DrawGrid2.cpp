// only include OpenGL if you use a custom drawer. Otherwise,
// there is no need to include any extra header
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/graphics.hpp>
#include <fantom/fields.hpp>
using namespace fantom;

#include "VisHelper.h"
#include "Delaunay.h"

#define MAXPOINTS 1024*1024

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
            add<Color>("Color", "If black it gets colored", Color(0,1,0));
            add<bool>("AsGrid", "", false);
            add<double>("SizeFactor", "", 1.0);
            add<double>("Distance", "", 4);

        }
    };


    DrawGridAlgorithm(InitData& data) : VisAlgorithm(data)
    {
    }

	Color getRGBColorFromTemperature(double min, double max, double temperature) {
	    if (temperature < min) temperature = min;
	    else if (temperature > max) temperature = max;

	    temperature = (temperature - min) / (max - min);
	    double r = std::min(std::max(0.0, 1.5 - fabs(1.0 - 4.0 * (temperature - 0.5))), 1.0);
	    double g = std::min(std::max(0.0, 1.5 - fabs(1.0 - 4.0 * (temperature - 0.25))), 1.0);
	    double b = std::min(std::max(0.0, 1.5 - fabs(1.0 - 4.0 * (temperature))), 1.0);
	    //debugLog() << "temp = " << temperature << " rgb = " << r << " " << g << " " << b << std::endl;
	    Color color = Color(r, g, b);
	    return color;
	}

    virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
    {
        mTriangle.reset();
        mTriangle = getGraphics("triangle").makePrimitive();

        try
        {
            auto domain = options.get<DiscreteDomain<2> >("Domain");
            auto color = options.get<Color>("Color");
            auto type = options.get<bool>("AsGrid");
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
            }
            // ValueArray<Point2>& points = domain->points();
            infoLog() << "Number of Points " << domain->points().size() << "\n";

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

                for(size_t j = 0; j < tdel->num_triangles; j++ )
                {
                    double value0 = pointValueMap[std::pair<double, double>(tdel->points[tdel->tris[j * 3]].x, tdel->points[tdel->tris[j * 3]].y)];
                    double value1 = pointValueMap[std::pair<double, double>(tdel->points[tdel->tris[j * 3 + 1]].x, tdel->points[tdel->tris[j * 3 + 1]].y)];
                    double value2 = pointValueMap[std::pair<double, double>(tdel->points[tdel->tris[j * 3 + 2]].x, tdel->points[tdel->tris[j * 3 + 2]].y)];

                    double currentValueInScale0 = (value0 - leastValue) / distanceHighestLeast;
                    double currentValueInScale1 = (value1 - leastValue) / distanceHighestLeast;
                    double currentValueInScale2 = (value2 - leastValue) / distanceHighestLeast;

                    std::vector<Color> colorTriangle(3, Color(0.0, 0.0, 0.0, 1.0));

                    colorTriangle[0] = getRGBColorFromTemperature(leastValue, distanceHighestLeast+leastValue, value0);
                    colorTriangle[1] = getRGBColorFromTemperature(leastValue, distanceHighestLeast+leastValue, value1);
                    colorTriangle[2] = getRGBColorFromTemperature(leastValue, distanceHighestLeast+leastValue, value2);

                    std::vector<Point3> tri(3);
                    tri[0] = Point3(tdel->points[tdel->tris[j * 3]].x, tdel->points[tdel->tris[j * 3]].y,
                            currentValueInScale0 * sizeFactor);
                    tri[1] = Point3(tdel->points[tdel->tris[j * 3 + 1]].x, tdel->points[tdel->tris[j * 3 + 1]].y,
                            currentValueInScale1 * sizeFactor);
                    tri[2] = Point3(tdel->points[tdel->tris[j * 3 + 2]].x, tdel->points[tdel->tris[j * 3 + 2]].y,
                            currentValueInScale2 * sizeFactor);

		    if (type) {
			mTriangle->add( Primitive::LINE_LOOP )
                                    .setLineWidth(1.0)
                                    .setColor(color == Color(0.0, 0.0, 0.0, 1.0) ? colorTriangle[0] : color)
                                    .setVertices(tri);
		    } else {
                        mTriangle->add(Primitive::TRIANGLE_STRIP).setColors(colorTriangle).setVertices(tri);
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

AlgorithmRegister<DrawGridAlgorithm> dummy("Grid/DrawGrid2", "Show some example graphics.");
} // namespace
