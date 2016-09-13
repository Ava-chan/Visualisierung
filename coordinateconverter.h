#ifndef CCOORDINATECONVERTER_H
#define CCOORDINATECONVERTER_H

#define _USE_MATH_DEFINES
#include <cmath>

class cCoordinateConverter
{
  public:
    void depthToWorld(int depthX, int depthY, int depthZ,
                      float& worldX, float& worldY, float& worldZ);

    cCoordinateConverter();
    cCoordinateConverter(int scrCntrX, int scrCntrY, float hFV, float vFV);

  private:
    const int screenHeight;
    const int screenWidth;

    const float horizontalFieldOfView;
    const float verticalFieldOfView;

    int screenCenterX;
    int screenCenterY;

    float f_x;
    float f_y;

    float rawDepthToMeters(int rawDepth);
    float degToRad(float degValue);

    void setScreenCenter();
    void setCoeffXAndCoeffY();
};

#endif // CCOORDINATECONVERTER_H
