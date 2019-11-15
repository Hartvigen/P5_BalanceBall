#include "main.h"

bool ballFound = false;
int16_t xCo, yCo;
int8_t xAng = 0, yAng = 0;

uint64_t startTime, endTime;


int main()
{
    initialize();
    setup();

#if USE_IMG_DIS
    SerialHelper::SendInt(1);
    while(true)
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

    #if INTF_RVIEWER
    SerialHelper::SendInt();
    #endif
}

void setup()
{
    MotorsController::Init();
    MotorsController::Reset();
    MotorsController::SetInnerSpeed(10);
    MotorsController::SetOuterSpeed(10);
    
#if !CTRL_MANUAL
    #if CTRL_PID
    initPID();
    #elif CTRL_AI
    // Init AI
    #endif
#endif

    CameraController::Init(CAM_SLAVE_PIN);
    CameraController::ManualCalibrate(3,6,3);
}


void loop()
{
    startTime = millis();
    ballFound = CameraController::GetBallLocation(xCo, yCo);

    if (ballFound)
    {
        #if CTRL_PID
        runPID(xCo, yCo, xAng, yAng);
        #elif CTRL_AI
        // Use AI
        #endif
    }

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
    Serial.print("Desired angles: Inner = "); Serial.print(xAng); Serial.print(" Outer = "); Serial.println(yAng);
#elif INTF_RVIEWER
    
#endif
}
