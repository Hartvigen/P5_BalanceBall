#ifndef UNIT_TEST
#include "main.h"

#include <stdint.h>
#include <Arduino.h>
#include <ArduCAM.h>
#include <SPI.h>

//Declaration of Variables used for the tracking and tilting processes.
int16_t xCo, yCo;
bool ballFound;
int8_t innerAng, outerAng;

//Variables used for timing the "loop" function.
uint64_t startTime, endTime, timer;

int main()
{
    initialize();
    setup();

//See "Setup.h" for explanation of values used in pre-processor directive.
#if USE_IMG_DIS
    while (true)
        CameraController::SendImageToProcessing();
#elif !CTRL_MANUAL
    timer = micros();
    while (true)
        loop();
#endif

    return 0;
}

inline void WaitTimer()
{
    while (micros() < timer) { }
    timer += 8000;
}

//Function initializes the Arduino board, and prepares all communication channels
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

//Function representing the major cycle of the schedule
inline void loop()
{
    MotorsController::Move();
    CameraController::BeginCapture();

    endTime = millis();
    printInfo();
    startTime = millis();

    for (int i = 9; i--;)
    {
        WaitTimer();
        MotorsController::Move();
    }

    CameraController::StartTracking();
    for (int i = 13; i--;)
    {
        CameraController::ProceedTracking(3);
        WaitTimer();
        MotorsController::Move();
    }
    ballFound = CameraController::EndTracking(xCo, yCo);

    if (ballFound)
    {
#if CTRL_PD
        PDController::RunPD(xCo, yCo, innerAng, outerAng);
#elif CTRL_AI
        AIController::RunNN(xCo, yCo, innerAng, outerAng);
#endif
        MotorsController::SetInnerAngle(innerAng);
        MotorsController::SetOuterAngle(outerAng);
    }
    else
    {
        MotorsController::SetInnerAngle(0);
        MotorsController::SetOuterAngle(0);
    }

    WaitTimer();
}


inline void printInfo()
{
#if INTF_TERMINAL
    Serial.print("Time: ");
    Serial.println((uint32_t)(endTime - startTime));
    Serial.print("Ball: ");
    if (ballFound)
    {
        Serial.print(xCo);
        Serial.print(" ");
        Serial.println(yCo);
    }
    else
    {
        Serial.println("Tracking...");
    }

    Serial.print("Actual angles: Inner = ");
    Serial.print(-MotorsController::GetInnerEncoder());
    Serial.print(" Outer = ");
    Serial.println(MotorsController::GetOuterEncoder());
    Serial.print("Desired angles: Inner = ");
    Serial.print(-innerAng);
    Serial.print(" Outer = ");
    Serial.println(outerAng);
#elif INTF_RVIEWER
    SerialHelper::SendInt((int32_t)(endTime - startTime));
    SerialHelper::SendInt((int32_t)ballFound);
    SerialHelper::SendInt((int32_t)xCo);
    SerialHelper::SendInt((int32_t)yCo);
    SerialHelper::SendInt(-(int32_t)MotorsController::GetInnerEncoder());
    SerialHelper::SendInt((int32_t)MotorsController::GetOuterEncoder());
    SerialHelper::SendInt(-(int32_t)innerAng);
    SerialHelper::SendInt((int32_t)outerAng);
#endif
}
#endif