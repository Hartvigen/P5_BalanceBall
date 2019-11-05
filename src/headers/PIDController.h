#ifndef PIDController_h
#define PIDController_h

#include <PID_v1.h>
#include <Arduino.h>

void initPID();
void runPID(float xCo, float yCo);

#endif