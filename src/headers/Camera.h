#ifndef Camera_h
#define Camera_h

#include <Arduino.h>
#include <ArduCAM.h>

void initCam();
void calibrateCam();
void calibrateFromImageRow();
void getBallLocation(float& xCo, float& yCo);
void readImage(float& xCo, float& yCo);
void readImageRow(uint16_t& pointCount, float& xCo, float& yCo, uint16_t  rowNumber);
void skipImageRows();
void skipImageRow();


#endif