#include <fstream>
#include <sstream>
#include <vector>
#include <utility>

#include <fantom/algorithm.hpp>
#include <fantom/fields.hpp>
#include <fantom/register.hpp>
using namespace fantom;

#include "InvDistWeighting.hpp"
#include "ModInvDistWeighting.hpp"

#include "VisHelper.h"


namespace
{

/**
    * Interpolates scattered data values on another set of points
    */
class InterpolateGeoDataAlgorithm : public DataAlgorithm
{
public:

    struct Options : public DataAlgorithm::Options
    {
    public:
        Options(Algorithm::Options::Control& control) : DataAlgorithm::Options(control)
        {
            // data to get interpolate
            add<DomainBase>("MeasuringPoints", "A point set or grid");
            add<TensorFieldDiscrete<Scalar> >("MeasuredValues", "A scattered scalar field");

            // sampling points
            add<DomainBase>("SamplingPoints", "A point set or grid");
        }
    };


    struct DataOutputs : public DataAlgorithm::DataOutputs
    {
    public:
        DataOutputs(Control& control) : DataAlgorithm::DataOutputs(control)
        {
            add<DomainBase>("SamplingPoints");
            add<TensorFieldBase>("InterpolatedValues");
        }
    };


    InterpolateGeoDataAlgorithm(InitData& data) : DataAlgorithm(data)
    {
    }


    void execute(const Algorithm::Options& options, const volatile bool& abortFlag)
    {
        if (abortFlag)
        {
            return;
        }

        // ////////////////////////////////////////////////
        infoLog() << "Get MeasuringPoints" << std::endl;
        auto measuringPoints = options.get<DiscreteDomain<2> >("MeasuringPoints");
        if (!measuringPoints)
        {
            debugLog() << "Measuring Points not connected." << std::endl;
            return;
        }

        for (size_t i = 0; i < measuringPoints->numPoints(); ++i)
        {
            m_measuredPositions.push_back(measuringPoints->points()[i]);
        }

        infoLog() << "Get MeasuredValues" << std::endl;
        auto measuredValueField = options.get<TensorFieldDiscrete<Scalar> >("MeasuredValues");
        if (!measuredValueField)
        {
            debugLog() << "MeasuredValues not connected." << std::endl;
            return;
        }

        auto eval = measuredValueField->makeDiscreteEvaluator();
        for (size_t i=0; i<eval->numValues(); ++i)
        {
            m_measuredValues.push_back(eval->value(i)[0]);
        }

        // ////////////////////////////////////////////////
        infoLog() << "Get SamplingPoints" << std::endl;
        auto samplingPoints = options.get<DiscreteDomain<2> >("SamplingPoints");
        if (!samplingPoints)
        {
            debugLog() << "sampling point set not connected." << std::endl;
            return;
        }

        for (size_t i = 0; i < samplingPoints->numPoints(); ++i)
        {
            std::unique_ptr<Interpolator> idw(new InverseDistanceWeighting);
            for (size_t j=0; j<m_measuredValues.size(); ++j)
            {
                idw->add(m_measuredValues[j], VisHelper::euclideanDistance(
                                                    samplingPoints->points()[i],
                                                    m_measuredPositions[j]));
            }

            m_interpolatedPositions.push_back(samplingPoints->points()[i]);
            m_interpolatedValues.push_back(Scalar((*idw)()));
        }

        // ////////////////////////////////////////////////
        infoLog() << "Serve DataOutputs" << std::endl;
        auto fieldInterpolatedValues  = DomainFactory::makeTensorField(*samplingPoints, m_interpolatedValues);;
        setResult("SamplingPoints", samplingPoints);
        setResult("InterpolatedValues", fieldInterpolatedValues);
    }

private:
    std::vector<Point2> m_measuredPositions;
    std::vector<double> m_measuredValues;
    std::vector<Point2> m_interpolatedPositions;
    std::vector<Scalar> m_interpolatedValues;
};

AlgorithmRegister<InterpolateGeoDataAlgorithm> dummy( "Interpolate", "Interpolate in a data set." );

} // namespace
