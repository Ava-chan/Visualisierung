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
                add<int>("Minima_difference", "", 8);
            }
        };


        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
        public:
            DataOutputs( Control& control ) : DataAlgorithm::DataOutputs( control )
            {
                add<DomainBase>("Points");
                add<TensorFieldBase>("Tiefenwerte");
                add<DomainBase>("Minima");
            }
        };


        LoadDepthDataAlgorithm(InitData& data) : DataAlgorithm(data)
        {
        }


        std::vector<cv::Point> bhContoursCenter(const std::vector<std::vector<cv::Point>>& contours,bool centerOfMass,int contourIdx=-1)
        {
            std::vector<cv::Point> result;
            if (contourIdx > -1)
            {
                if (centerOfMass)
                {
                    cv::Moments m = cv::moments(contours[contourIdx],true);
                    result.push_back( cv::Point(m.m10/m.m00, m.m01/m.m00));
                }
                else
                {
                    cv::Rect rct = cv::boundingRect(contours[contourIdx]);
                    result.push_back( cv::Point(rct.x + rct.width / 2 ,rct.y + rct.height / 2));
                }
            }
            else
            {
                if (centerOfMass)
                {
                    for (int i=0; i < contours.size();i++)
                    {
                        cv::Moments m = cv::moments(contours[i],true);
                        result.push_back( cv::Point(m.m10/m.m00, m.m01/m.m00));

                    }
                }
                else
                {

                    for (int i=0; i < contours.size(); i++)
                    {
                        cv::Rect rct = cv::boundingRect(contours[i]);
                        result.push_back(cv::Point(rct.x + rct.width / 2 ,rct.y + rct.height / 2));
                    }
                }
            }

            return result;
        }


        std::vector<cv::Point> bhFindLocalMaximum(cv::InputArray _src,int neighbor=2){
            cv::Mat src = _src.getMat();

            cv::Mat peak_img = src.clone();
            cv::dilate(peak_img,peak_img,cv::Mat(),cv::Point(-1,-1),neighbor);
            peak_img = peak_img - src;

            cv::Mat flat_img ;
            cv::erode(src,flat_img,cv::Mat(),cv::Point(-1,-1),neighbor);
            flat_img = src - flat_img;

            cv::threshold(peak_img,peak_img,0,255,CV_THRESH_BINARY);
            cv::threshold(flat_img,flat_img,0,255,CV_THRESH_BINARY);
            cv::bitwise_not(flat_img,flat_img);

            peak_img.setTo(cv::Scalar::all(255),flat_img);
            cv::bitwise_not(peak_img,peak_img);


            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(peak_img,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

            return bhContoursCenter(contours,true);
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

                //infoLog() << "Calculate local minima" << std::endl;

                std::vector<cv::Point> minima;

                std::vector<Point2> vecMinimaPositions;
                cv::Mat B = cv::imread(sFilename, 0);
                //invert picture
                cv::Mat A =  cv::Scalar::all(255) - B;
                //return minima, because the picture is inverted.
                minima = bhFindLocalMaximum(A, parameters.get<int>("Minima_difference"));
                for (int i = 0; i < minima.size(); i++) {
                    vecMinimaPositions.push_back(Point2(minima.at(i).x, minima.at(i).y));
                }

                auto minimaPoints  = DomainFactory::makeDomainArbitrary(vecMinimaPositions);
                setResult("Minima", minimaPoints);
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
