#include "rollingTable/MotorsController.h"
#include "rollingTable/CameraController.h"
#include <Wire.h>

using namespace RollingTable;

#define CAM_SLAVE_PIN 53

#define INTERVAL_TRACK (uint64_t)200;
uint64_t trackTime = 0;


void initialize();
void setup();
void loop();


int main()
{
    initialize();
    setup();

    digitalWrite(LED_BUILTIN, HIGH);
    while (true)
        loop();

    return 0;
}


void initialize()
{
    init();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Wire.begin();
    SPI.begin();
    Serial.begin(691200L);

    while (!Serial) { }
    while (Serial.available() > 0)
        Serial.read();
    delay(1000);
}

void setup()
{
    MotorsController::Init();
    MotorsController::Reset();
    MotorsController::SetInnerSpeed(10);
    MotorsController::SetOuterSpeed(10);

    CameraController::Init(CAM_SLAVE_PIN);
    CameraController::Recalibrate();
}

void loop()
{
    if (millis() > trackTime)
    {
        trackTime = millis() + INTERVAL_TRACK;

        int16_t xCo, yCo;
        int16_t xAng, yAng;

        CameraController::GetBallLocation(xCo, yCo);

        // Get xAng, yAng. This is where PID and AI will differ

        MotorsController::SetInnerAngle(xAng);
        MotorsController::SetOuterAngle(yAng);
    }

    MotorsController::Move();
}



/*
initCam();
calibrateCam();
while(Serial.available() == 0){}
Serial.read();
Serial.println("Starting...");
float xCo, yCo;
uint64_t nextCapture =  0;

{
    while(millis() < nextCapture){}
    nextCapture = millis() + CAPTURE_RATE;
    xCo = 0, yCo = 0;
    getBallLocation(xCo, yCo);
    Serial.print("X: ");
    Serial.print(xCo);
    Serial.print("    Y: ");
    Serial.println(yCo);
    

    //getBallLocation
    //Choose action (center of ball, current tilt, velocity of ball)
    //perform action (Set tilt)
}
*/
