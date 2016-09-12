#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/graphics.hpp>
#include <fantom/fields.hpp>
#include <fantom/outputs/VisOutputs.hpp>


using namespace fantom;

namespace {

    class ShowPointsAlgorithm : public VisAlgorithm {
        // Zeiger auf interne Grafikobjekte
        std::unique_ptr<Primitive> mPoints;
        std::unique_ptr<Primitive> mLabels;

    public:
        // Ausgaben der Visualisierung bekanntmachen
        struct VisOutputs : public VisAlgorithm::VisOutputs {
            VisOutputs(fantom::VisOutputs::Control &control)
                    : VisAlgorithm::VisOutputs(control) {
                // Zwei benannte Grafikobjekte
                addGraphics("points");
                addGraphics("labels");
            }
        };

        struct Options
                : public DataAlgorithm::Options {
        public:
            Options(Algorithm::Options::Control &control)
                    : DataAlgorithm::Options(control) {
                // Die Datenpunkte
                add<All_DiscreteDomain>("Domain", "");

                // Zusätzliche Optionen, wie Farbe und Goesse
                add<Color>("Color", "", Color(1, 0, 0));
                add<double>("Size", "", 1.0);
                add<bool>("Show Labels", "", false);

                // Optional ein Feld mit Werten
                add<TensorFieldDiscrete<Scalar> >("Field", "");
                add<double>("min Temperature", "", -1.0);
                add<double>("max Temperature", "", 15);
                add<double>("min shown Temp", "", -80);
                add<double>("max shown Temp", "", 80);
            }
        };


        ShowPointsAlgorithm(InitData &data)
                : VisAlgorithm(data) {
        }

        // Create a Color out of a temperature with a gradient between -20 and +40
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

        virtual void execute(const Algorithm::Options &options, const volatile bool & /*abortFlag*/ ) override {
            // loeschen der aktuellen Grafik und anlegen neuer Grafikobjekte
            mPoints.reset();
            mPoints = getGraphics("points").makePrimitive();

            mLabels.reset();
            mLabels = getGraphics("labels").makePrimitive();

            // Lesen der parameter
            auto color = options.get<Color>("Color");

            auto domain = options.get<DiscreteDomain<2> >("Domain");

            auto field = options.get<const TensorFieldDiscrete<Scalar> >("Field");

            //debugLog() << "exec" << std::endl;
            // Testen welche Eingaben gesetzt sind
            if (field) {
                //debugLog() << "field with values" << std::endl;
                // Punkte + Werte?
                // Zeige beides an
                auto domain = std::dynamic_pointer_cast<const DiscreteDomain<2> >(field->domain());

                auto eval = field->makeDiscreteEvaluator();
                if (domain) {
                    //debugLog() << "read parameters" << std::endl;
                    double minTemp = options.get<double>("min Temperature");
                    double maxTemp = options.get<double>("max Temperature");
                    double filterMin = options.get<double>("min shown Temp");
                    double filterMax = options.get<double>("max shown Temp");
                    for (size_t i = 0; i < domain->numPoints(); ++i) {
                        //Temperatur wird zwischen -20 und +40 °C in Farbe gewandelt
                        if (field->values()[i][0] >= filterMin && field->values()[i][0] <= filterMax) {
                            mPoints->addSphere(toPoint3(domain->points()[i]), options.get<double>("Size"),
                                               getRGBColorFromTemperature(minTemp, maxTemp, field->values()[i][0]));
                            if (options.get<bool>("Show Labels"))
                                mLabels->addTextLabel(toPoint3(domain->points()[i]), std::to_string(i), 24);
                        }
                    }
                }
            } else if (domain) {
                //debugLog() << "domain" << std::endl;
                // Nur Punkte? Zeige Punkte an.
                for (size_t i = 0; i < domain->numPoints(); ++i) {
                    mPoints->addSphere(toPoint3(domain->points()[i]), options.get<double>("Size"), color);
                    if (options.get<bool>("Show Labels"))
                        mLabels->addTextLabel(toPoint3(domain->points()[i]), std::to_string(i), 24);
                }
            }
        }
    };

    AlgorithmRegister<ShowPointsAlgorithm> dummy("Geo Data/Show Points", "Show some example graphics.");
} // namespace

