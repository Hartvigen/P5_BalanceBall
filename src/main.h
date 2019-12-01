#ifndef UNIT_TEST
#include "Setup.h"

#if USE_IMG_DIS || INTF_RVIEWER
#include "Serial/SerialHelper.h"
#endif

#include "rollingTable/PDController.h"
#include "rollingTable/AIController.h"
#include "rollingTable/MotorsController.h"
#include "rollingTable/CameraController.h"
#include <Wire.h>
#include <SPI.h>

using namespace RollingTable;

#define CAM_SLAVE_PIN 53

void initialize();
void setup();
void loop();

void printInfo();
#endif