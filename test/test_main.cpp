#ifdef UNIT_TEST
#include <Wire.h>
#include "../../src/Serial/SerialHelper.h"
#include "test_rollingTable/test_CameraController.h"
#include "test_rollingTable/test_MotorsController.h"
#include "test_rollingTable/test_PIDController.h"
#include "test_rollingTable/test_AIController.h"

#define CAM_SLAVE_PIN 53

void initialize()
{
    init();

    Wire.begin();
    SPI.begin();
    Serial.begin(691200L);

    while (!Serial) { }
    while (Serial.available() > 0)
        Serial.read();
    delay(1000);

//If desired, starts communication with viewing program.
#if INTF_RVIEWER || USE_IMG_DIS
    SerialHelper::SendInt(IMAGE_HEIGHT);
    SerialHelper::SendInt(IMAGE_WIDTH);
    SerialHelper::SendInt(MAX_ANGLE);
#endif
}

//Function initializes controllers.
void setup()
{
    CameraController::Init(CAM_SLAVE_PIN, 4, 8, 4);
    MotorsController::Init();
    
}

int main()
{
    
    initialize();
    setup();
    
    UNITY_BEGIN();
    
    TestAll_CameraController();
    TestAll_MotorsController();
    TestAll_PIDController();
    TestAll_AIController();

    UNITY_END();
}

#endif