#include "main.h"

bool ballFound = false;
int16_t xCo, yCo;
int16_t xAng, yAng;

uint32_t time;


int main()
{
    initialize();
    setup();

#if USE_IMG_DIS
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
    CameraController::Recalibrate();
}


void loop()
{
    time = millis();
    ballFound = CameraController::GetBallLocation(xCo, yCo);
    Serial.print(millis() - time); Serial.print(" "); 
    
    if (ballFound)
    { 
        Serial.print(xCo); Serial.print(" "); Serial.println(yCo);

        #if USE_PID
        runPID(xCo, yCo);
        #else
        // Use AI
        #endif
    }
    else
    { 
        Serial.println("Tracking..."); 
    }

    MotorsController::SetInnerAngle(xAng);
    MotorsController::SetOuterAngle(yAng);

    //MotorsController::Move();
}
