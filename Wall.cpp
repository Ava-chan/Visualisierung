#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "lodepng.h"

#include <fantom/algorithm.hpp>
#include <fantom/fields.hpp>
#include <fantom/register.hpp>
using namespace fantom;


namespace
{

    /**
    * Loader for a simple tabular format with Geo/Meteo information
    * Each entry is a line
    * Each line contains (at least) the following data
    * latitude longitude Temperature
    * The first two lines are comment lines and are ignored
    */
    class LoadWallAlgorithm : public DataAlgorithm
    {
    public:

        struct Options : public DataAlgorithm::Options
        {
        public:
            Options(Algorithm::Options::Control& control) : DataAlgorithm::Options(control)
            {
                add<InputLoadPath>("Input depth file", "The file to be read", "");
                add<InputLoadPath>("Input color file", "The file to be read", "");
            }
        };


        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
        public:
            DataOutputs( Control& control ) : DataAlgorithm::DataOutputs( control )
            {
                add<DomainBase>("Points");
                add<TensorFieldBase>("depth");
                add<TensorFieldBase>("color");
            }
        };


        LoadWallAlgorithm( InitData& data ) : DataAlgorithm( data )
        {
        }


        void execute(const Algorithm::Options& parameters, const volatile bool& abortFlag)
        {
            // Test, ob die Dati gesetzt ist, sonst nichts machen
            if (parameters.get<std::string>("Input depth file") != "")
            {
                std::string filename = parameters.get<std::string>("Input depth file");

                // Speicher fuer die Daten
                std::vector<Point2> positions;
                std::vector<Scalar> depth;

                // Schleife zum einlesen der Daten
                std::vector<unsigned char> image; //the raw pixels
                unsigned width, height;

                //decode
                unsigned error = lodepng::decode(image, width, height, filename);

                //if there's an error, display it
                if(error) infoLog() << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

                infoLog() << "loaded picture with width " << width << " and height " << height << std::endl;
                int pointcount = 0;

                for (unsigned h = 0; h < height && !abortFlag; h++) {
                    for (unsigned w = 0; w < width && !abortFlag; w++) {
                    //infoLog() << "w " << w << " - gray value " << ((((int)image.at(w * 4 + 0)) + ((int)(image.at(w * 4 + 1)) << 8)) >> 4) << std::endl;
                        int dep = ((((int)image.at((w + h * width) * 4 + 0)) + ((uint8_t)(image.at((w + h * width) * 4 + 1)) << 8)) >> 4);
                        //if (dep != 0) {
                            positions.push_back(Point2(w, height-h));
                            depth.push_back(Scalar(dep));
                            pointcount++;
                        //}
                    }
                }

                infoLog() << "loaded " << pointcount << " points " << std::endl;



                auto domain = DomainFactory::makeDomainArbitrary(positions);
                auto fieldDepth  = DomainFactory::makeTensorField(*domain, depth);

                setResult("Points", domain);
                setResult("depth", fieldDepth);
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
                    options.set<std::string>("Input depth file", *it);
                    it = filenames.erase(it);
                    return true;
                }
            }
            return false;
        }
    };

    AlgorithmRegister<LoadWallAlgorithm> dummy("Wall", "Load depth and color informations for a wall representation");
    DataLoaderRegister<LoadWallAlgorithm> dummy2("Wall");
} // namespace