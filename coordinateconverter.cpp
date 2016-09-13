#include "coordinateconverter.h"


cCoordinateConverter::cCoordinateConverter()
  : screenHeight(424), screenWidth(512), horizontalFieldOfView(70.6f), verticalFieldOfView(60.0f)
{
  setScreenCenter();
  setCoeffXAndCoeffY();
}


cCoordinateConverter::cCoordinateConverter(int scrHghtX, int scrWdthY, float hFV, float vFV)
  : screenHeight(scrHghtX), screenWidth(scrWdthY), horizontalFieldOfView(hFV), verticalFieldOfView(vFV)
{
  setScreenCenter();
  setCoeffXAndCoeffY();
}


void cCoordinateConverter::setScreenCenter()
{
  screenCenterX = screenWidth / 2;
  screenCenterY = screenHeight / 2;
}


void cCoordinateConverter::setCoeffXAndCoeffY()
{
  float xzFactor = (2.0f * tan(degToRad(horizontalFieldOfView/2.0f)));
  float yzFactor = (2.0f * tan(degToRad(verticalFieldOfView/2.0f)));

  f_x = static_cast<float>(screenWidth) / xzFactor;
  f_y = static_cast<float>(screenHeight) / yzFactor;
}


void cCoordinateConverter::depthToWorld(int depthX, int depthY, int depthZ,
                                        float& worldX, float& worldY, float& worldZ)
{
  worldZ = rawDepthToMeters(depthZ);

  if ((screenCenterX - depthX) && (screenCenterY - depthY))
  {
    worldX = worldZ * static_cast<float>(screenCenterX - depthX) / f_x;
    worldY = worldZ * static_cast<float>(screenCenterY - depthY) / f_y;

    float flupp = worldX;
    worldX = worldY;
    worldY = flupp;
  }
  else
  {
    worldX = 0.0f;
    worldY = 0.0f;
    worldZ = 0.0f;
  }
}


float cCoordinateConverter::rawDepthToMeters(int rawDepth)
{
  float rawDepthF = static_cast<double>(rawDepth);

  if (rawDepth <= 0xFFFF)
  {
    // following values acquired during kinect v2 depth sensor calibration
    // xtion value -> float depth = 3.33776840956e-05f * rawDepthF + -0.0674169620271f;
    float depth = (0.012432f * rawDepthF + -1.475340f) / 100.0f;

    return (depth);
  }
  return 0.0f;
}


float cCoordinateConverter::degToRad(float degValue)
{
  return degValue * M_PI / 180.0f;
}
