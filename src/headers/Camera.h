#ifndef Camera_h
#define Camera_h

#include <NXTCam.h>
#include <Wire.h>
#include <NXShield.h>

#include <util/delay.h> // header file for delay.

extern NXTCam startAndReturnCamera(NXShield);
extern void getAllBlobs(NXTCam, int*, int*, int*);

#endif