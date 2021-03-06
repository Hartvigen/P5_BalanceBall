#include "main.h"

bool ballFound = false;
int16_t xCo, yCo;
int8_t xAng = 0, yAng = 0;

uint32_t time;


int main()
{
    initialize();
    setup();

#if USE_IMG_DIS
    SerialHelper::SendInt(1);
    while(true)
        CameraController::SendImageToProcessing();
#else
    while (true)
        loop();
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
}

void setup()
{
    MotorsController::Init();
    MotorsController::Reset();
    MotorsController::SetInnerSpeed(10);
    MotorsController::SetOuterSpeed(10);
    
    #if USE_PID
    initPID();
    #else
    // Init AI
    #endif

    CameraController::Init(CAM_SLAVE_PIN);
    CameraController::ManualCalibrate(3,6,3);
}


void loop()
{
    time = millis();
    ballFound = CameraController::GetBallLocation(xCo, yCo);
    Serial.print(millis() - time); Serial.print(" "); 
    delay(10);
    if (ballFound)
    { 
        Serial.print(xCo); Serial.print(" "); Serial.print(yCo);

        #if USE_PID
        runPID(xCo, yCo, xAng, yAng);

        MotorsController::SetInnerAngle(xAng);
        MotorsController::SetOuterAngle(0);

        Serial.print(" Output: ");
        Serial.println(xAng);
        
        MotorsController::Move();
        #else
        // Use AI
        #endif
    }
    else
    { 
        Serial.println("Tracking..."); 
    }

}
