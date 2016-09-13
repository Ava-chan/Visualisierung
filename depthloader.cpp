#include "coordinateconverter.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <fantom/algorithm.hpp>
#include <fantom/fields.hpp>
#include <fantom/register.hpp>
using namespace fantom;


namespace
{

    class LoadDepthDataAlgorithm : public DataAlgorithm
    {
    public:

        struct Options : public DataAlgorithm::Options
        {
        public:
            Options(Algorithm::Options::Control& control) : DataAlgorithm::Options(control)
            {
                add<InputLoadPath>("Input File", "The file to be read", "");
            }
        };


        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
        public:
            DataOutputs( Control& control ) : DataAlgorithm::DataOutputs( control )
            {
                add<DomainBase>("Points");
                add<TensorFieldBase>("Tiefenwerte");
            }
        };


        LoadDepthDataAlgorithm(InitData& data) : DataAlgorithm(data)
        {
        }


        void execute(const Algorithm::Options& parameters, const volatile bool& abortFlag)
        {
            if (parameters.get<std::string>("Input File") != "")
            {
                std::string sFilename = parameters.get<std::string>("Input File");
                cv::Mat oDepthImage = cv::imread(sFilename, CV_LOAD_IMAGE_ANYDEPTH);

                infoLog() << sFilename << " (" << oDepthImage.rows << ", " << oDepthImage.cols << ")\n";

                std::vector<Point2> vecPositions;
                std::vector<Scalar> vecDepthValues;

                for (unsigned nRow=0; nRow<oDepthImage.rows; ++nRow)
                {
                    for (unsigned nCol=0; nCol<oDepthImage.cols; ++nCol)
                    {
                        float fWorldX, fWorldY, fWorldZ;
                        cCoordinateConverter oCoordinateConverter;
                        oCoordinateConverter.depthToWorld(nRow, nCol, oDepthImage.at<unsigned short>(nRow, nCol),
                                                          fWorldX, fWorldY, fWorldZ);
                        vecPositions.push_back(Point2(fWorldX, fWorldY));
                        vecDepthValues.push_back(Scalar(fWorldZ));
                    }
                }

                auto domain = DomainFactory::makeDomainArbitrary(vecPositions);
                auto fieldTemperature  = DomainFactory::makeTensorField(*domain, vecDepthValues);

                setResult("Points", domain);
                setResult("Tiefenwerte", fieldTemperature);
            }
        }


        void CreatePointCloudFromImage(const cv::Mat& oDepthImage)
        {
            for (unsigned nRow=0; nRow<oDepthImage.rows; ++nRow)
            {
                for (unsigned nCol=0; nCol<oDepthImage.cols; ++nCol)
                {
                    float fWorldX, fWorldY, fWorldZ;
                    cCoordinateConverter oCoordinateConverter;
                    oCoordinateConverter.depthToWorld(nRow, nCol, oDepthImage.at<unsigned short>(nRow, nCol),
                                                      fWorldX, fWorldY, fWorldZ);

                }
            }
        }


        IMPLEMENT_CAN_HANDLE(".png")

        static bool loaderSetOptions(Algorithm::Options& options, std::vector<std::string>& filenames)
        {
            // find the name we will handle
            for (auto it = filenames.begin(); it != filenames.end(); ++it)
            {
                if (it->substr(it->size()-4, 4) == ".png")
                {
                    options.set<std::string>("Input File", *it);
                    it = filenames.erase(it);
                    return true;
                }
            }
            return false;
        }
    };

    AlgorithmRegister<LoadDepthDataAlgorithm> dummy("Load/DepthData", "Load a test DepthData dataset.");
    DataLoaderRegister<LoadDepthDataAlgorithm> dummy2("Load/DepthData");
} // namespace
