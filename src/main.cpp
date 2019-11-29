#include "main.h"

//Declaration of Variables used for the tracking and tilting processes.
int16_t xCo, yCo;
bool ballFound;
int8_t innerAng, outerAng;

//Variables used for timing the "loop" function.
uint64_t startTime, endTime;

int main()
{
    initialize();
    setup();

//See "Setup.h" for explanation of values used in pre-processor directive.
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
    SerialHelper::SendInt(1);
    #endif
}

//Function initializes program controllers.
void setup()
{
    CameraController::Init(CAM_SLAVE_PIN, 3,6,3);

    MotorsController::Init();
    MotorsController::Reset();

    #if !CTRL_MANUAL && CTRL_AI
    // Init AI
    #endif
}

//function representing the major cycle of the schedule
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

    #if CTRL_PD
    PDController::RunPD(xCo, yCo, innerAng, outerAng);
    #elif CTRL_AI
    // AIController::RunAI();
    #endif

    MotorsController::SetInnerAngle(innerAng);
    MotorsController::SetOuterAngle(outerAng);
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
    Serial.print("Desired angles: Inner = "); Serial.print(innerAng); Serial.print(" Outer = "); Serial.println(outerAng);
#elif INTF_RVIEWER
    
#endif
}
