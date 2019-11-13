#ifndef PIDController_h
#define PIDController_h

#include <PID_v1.h>
#include <Arduino.h>
#include "rollingTable/MotorsController.h"

void initPID();
void runPID(float xCo, float yCo, int8_t &xAng, int8_t &yAng);

#endif