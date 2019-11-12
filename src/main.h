#include "Setup.h"

#include "rollingTable/MotorsController.h"
#include "rollingTable/CameraController.h"
#include <Wire.h>

#include "headers/PIDController.h"

using namespace RollingTable;


#define CAM_SLAVE_PIN 53
#define INTERVAL_TRACK (uint64_t)200;
uint64_t trackTime = 0;


void initialize();
void setup();
void loop();
void track();