#ifndef Camera_h
#define Camera_h

#include <NXShield.h>

#include <util/delay.h> // header file for delay.
#include <Wire.h>
#include <BreezyArduCAM.h>
#include <SPI.h>

extern Arducam initCam();
extern void runCameraTest();

#endif