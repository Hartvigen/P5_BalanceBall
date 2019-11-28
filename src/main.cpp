#ifndef UNIT_TEST
#include "main.h"

int16_t xCo, yCo;
bool ballFound = false;
int8_t xAng = 0, yAng = 0;

uint64_t startTime, endTime;


int main()
{
    initialize();
    setup();

#if USE_IMG_DIS
    while (true)
        CameraController::SendImageToProcessing();
#elif !CTRL_MANUAL
    while (true)
        loop();
#else

#endif

    return 0;
}


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


void loop()
{
    startTime = millis();

    CameraController::StartTracking();
    for (int i = 13; i--;)
    {
        CameraController::ProceedTracking(3);
        MotorsController::Move();
    }
    ballFound = CameraController::EndTracking(xCo, yCo);

    #if CTRL_PID
    PIDController::RunPID(xCo, yCo, xAng, yAng);
    #elif CTRL_AI
    // AIController::RunAI();
    #endif

    MotorsController::SetInnerAngle(xAng);
    MotorsController::SetOuterAngle(yAng);
    MotorsController::Move();
    
    endTime = millis();
    printInfo();
}


void printInfo()
{
#if INTF_TERMINAL
    Serial.print("Time: "); Serial.println((uint32_t)(endTime - startTime));
    Serial.print("Ball: "); 
    if (ballFound) { Serial.print(xCo); Serial.print(" "); Serial.println(yCo); }
    else { Serial.println("Tracking..."); }

    Serial.print("Actual angles: Inner = "); Serial.print(MotorsController::GetInnerEncoder()); Serial.print(" Outer = "); Serial.println(MotorsController::GetOuterEncoder());
    Serial.print("Desired angles: Inner = "); Serial.print(xAng); Serial.print(" Outer = "); Serial.println(yAng); // May show too large or small values, I know
#elif INTF_RVIEWER
    
#endif
}
#endif