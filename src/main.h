#include "Setup.h"

#include "headers/PIDController.h"
#include "rollingTable/MotorsController.h"
#include "rollingTable/CameraController.h"
#include <Wire.h>

using namespace RollingTable;

#define CAM_SLAVE_PIN 53

void initialize();
void setup();
void loop();