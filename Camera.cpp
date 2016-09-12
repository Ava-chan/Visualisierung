#include <fantom/algorithm.hpp>
#include <fantom/register.hpp>
#include <fantom/graphics.hpp>
#include <fantom/fields.hpp>
#include <fantom/outputs/VisOutputs.hpp>


using namespace fantom;

namespace {

    class CameraAlgorithm : public VisAlgorithm {
        // Zeiger auf interne Grafikobjekte
        //std::unique_ptr<Primitive> mPoints;
        std::unique_ptr<Primitive> mTriangle;

    public:
        // Ausgaben der Visualisierung bekanntmachen
        struct VisOutputs : public VisAlgorithm::VisOutputs {
            VisOutputs(fantom::VisOutputs::Control &control)
                    : VisAlgorithm::VisOutputs(control) {
                // Zwei benannte Grafikobjekte
                //addGraphics("points");
                addGraphics("triangle");
            }
        };

        struct Options
                : public DataAlgorithm::Options {
        public:
            Options(Algorithm::Options::Control &control)
                    : DataAlgorithm::Options(control) {
                // Ein Feld mit Werten
                add<TensorFieldDiscrete<Scalar> >("field", "");
            }
        };


        CameraAlgorithm(InitData &data)
                : VisAlgorithm(data) {
        }

        virtual void execute(const Algorithm::Options &options, const volatile bool & abortFlag ) override {
            // loeschen der aktuellen Grafik und anlegen neuer Grafikobjekte
            //mPoints.reset();
            //mPoints = getGraphics("points").makePrimitive();
            mTriangle.reset();
            mTriangle = getGraphics("triangle").makePrimitive();

            // Lesen der parameter
            auto field = options.get<const TensorFieldDiscrete<Scalar> >("field");

            //debugLog() << "exec" << std::endl;
            // Testen welche Eingaben gesetzt sind
            if (field) {
                //debugLog() << "field with values" << std::endl;
                // Punkte + Werte?
                // Zeige beides an
                auto domain = std::dynamic_pointer_cast<const DiscreteDomain<2> >(field->domain());

                auto eval = field->makeDiscreteEvaluator();
                std::vector<Point3> tri(3);
                if (domain) {
                    int width = 512, height = 424;
                    int stepsize = 1;
                    int eps = 10;
                    float colorrange = 4096.0;
                    float deepinterpret = 8;

                    bool first;
                    int pitch;
                    int k;

                    for (int h = 0; h < height - stepsize && !abortFlag; h+=stepsize) {
                        first = true;
                        std::vector<Color> colorTriangle(3, Color(1.0, 1.0, 1.0, 1.0));
                        for (int w = 0; w < width - stepsize && !abortFlag; w+=stepsize) {
                            size_t i = h * width + w;
                            if (field->values()[i][0] != 0) {
                                int newpitch = field->values()[i + stepsize][0] - field->values()[i][0];

                                if (!first) {
                                    if (newpitch > pitch - eps && newpitch < pitch + eps) {
                                        tri[1] = toPoint3(domain->points()[i + stepsize]);
                                        tri[1][2] = (float)field->values()[i + stepsize][0] / deepinterpret;
                                        pitch = pitch + (newpitch - pitch) / k;
                                        float color = (float)(field->values()[i + stepsize][0]) / colorrange;

                                        colorTriangle[1] = Color(color, color, color);
                                        k++;
                                    } else {
                                        mTriangle->add(Primitive::TRIANGLE_STRIP)
                                                .setLineWidth(0.0)
                                                .setColors(colorTriangle)
                                                .setVertices(tri);
                                        first = true;
                                    }
                                }
                                if (first) {
                                    tri[0] = toPoint3(domain->points()[i]);
                                    tri[0][2] = (float)field->values()[i][0] / deepinterpret;
                                    float color = (float)(field->values()[i][0]) / colorrange;
                                    colorTriangle[0] = Color(color, color, color);

                                    tri[1] = toPoint3(domain->points()[i + stepsize]);
                                    tri[1][2] = (float)field->values()[i + stepsize][0]  / deepinterpret;
                                    color = ((float)field->values()[i + stepsize][0]) / colorrange;
                                    colorTriangle[1] = Color(color, color, color);

                                    tri[2] = toPoint3(domain->points()[i + stepsize * width]);
                                    tri[2][2] = (float)field->values()[i + stepsize * width][0]  / deepinterpret;
                                    color = ((float)field->values()[i + stepsize * width][0]) / colorrange;
                                    colorTriangle[2] = Color(color, color, color);
                                    first = false;
                                    pitch = newpitch;
                                    k = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    };

    AlgorithmRegister<CameraAlgorithm> dummy("Camera", "Do the Camerastuff");
} // namespace

