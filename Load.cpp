#include <fstream>
#include <sstream>
#include <vector>

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
    class LoadGeoDataAlgorithm : public DataAlgorithm
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
                add<TensorFieldBase>("Temperature");
                add<TensorFieldBase>("Rainfall");
                add<TensorFieldBase>("Sunshine");
            }
        };


        LoadGeoDataAlgorithm( InitData& data ) : DataAlgorithm( data )
        {
        }


        void execute(const Algorithm::Options& parameters, const volatile bool& abortFlag)
        {
            // Test, ob die Dati gesetzt ist, sonst nichts machen
            if (parameters.get<std::string>("Input File") != "")
            {
                std::ifstream infile(parameters.get<std::string>("Input File"));

                // Fehler werfen, wenn die Datei ungueltig ist
                if (!infile)
                {
                    throw "Input file cannot be opened";
                }

                std::string line;

                // skip first two lines
                getline(infile, line);
                getline(infile, line);

                // Speicher fuer die Daten
                std::vector<Point2> positions;
                std::vector<Scalar> temperatures;
                std::vector<Scalar> rainfalls;
                std::vector<Scalar> sunshines;

                // Schleife zum einlesen der Daten
                int i = 0;

                while (getline(infile, line))
                {
                    std::stringstream str(line);
                    double lat;
                    double lon;
                    double temperatur;
                    double rainfall;
                    double sunshine;
                    str >> lat;
                    str >> lon;
                    str >> temperatur;
                    str >> rainfall;
                    str >> sunshine;
                    positions.push_back(Point2(lat, lon));
                    temperatures.push_back(Scalar(temperatur));
                    rainfalls.push_back(Scalar(rainfall));
                    sunshines.push_back(Scalar(sunshine));
                    infoLog() << "Point" << i++ << " (" << lat << ", " << lon << ")\n";
                }

                auto domain = DomainFactory::makeDomainArbitrary(positions);
                auto fieldTemperature  = DomainFactory::makeTensorField(*domain, temperatures);
                auto fieldRainfall  = DomainFactory::makeTensorField(*domain, rainfalls);
                auto fieldSunshine = DomainFactory::makeTensorField(*domain, sunshines);

                setResult("Points", domain);
                setResult("Temperature", fieldTemperature);
                setResult("Rainfall", fieldRainfall);
                setResult("Sunshine", fieldSunshine);
            }
        }

        IMPLEMENT_CAN_HANDLE(".dat")

        static bool loaderSetOptions(Algorithm::Options& options, std::vector<std::string>& filenames)
        {
            // find the name we will handle
            for (auto it = filenames.begin(); it != filenames.end(); ++it)
            {
                if (it->substr(it->size()-4, 4) == ".dat")
                {
                    options.set<std::string>("Input File", *it);
                    it = filenames.erase(it);
                    return true;
                }
            }
            return false;
        }
    };

    AlgorithmRegister<LoadGeoDataAlgorithm> dummy("Load/GeoData", "Load a test GeoData dataset.");
    DataLoaderRegister<LoadGeoDataAlgorithm> dummy2("Load/GeoData");
} // namespace
