#ifdef UNIT_TEST
#include <Wire.h>
#include "../../src/main.h"
#include "test_rollingTable/test_CameraController.h"
#include "test_rollingTable/test_MotorsController.h"
#include "test_rollingTable/test_PIDController.h"

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

    #if INTF_RVIEWER || USE_IMG_DIS
    SerialHelper::SendInt(1);
    #endif
}

void setup()
{
    CameraController::Init(CAM_SLAVE_PIN, 3,6,3);

    MotorsController::Init();
    MotorsController::Reset();

    #if !CTRL_MANUAL && CTRL_AI
    // Init AI
    #endif
}

int main(){
    
    initialize();
    setup();
    
    UNITY_BEGIN();
    
    TestAll_CameraController();
    TestAll_MotorsController();
    TestAll_PIDController();

    UNITY_END();
}

#endif