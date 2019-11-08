#include "main.h"
#include "Serial/SerialHelper.h"

bool ballFound = false;
int16_t xCo, yCo;
int16_t xAng, yAng;

uint32_t time = 0;


int main()
{
    initialize();
    setup();

    digitalWrite(LED_BUILTIN, HIGH);
    while (true)
        if (ballFound)
            loop();
        else 
            track();

    //while(true)
    //    CameraController::SendImage();

    return 0;
}


void initialize()
{
    init();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Wire.begin();
    Wire.setClock((uint32_t)1000000);
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
    time = millis();
    ballFound = CameraController::GetBallLocation(xCo, yCo);
    Serial.print(millis() - time); Serial.print(" "); 

    if (ballFound)
    { Serial.print(xCo); Serial.print(" "); Serial.println(yCo); }
    else
    { Serial.println("Lost..."); }
    

    // Get xAng, yAng. This is where PID and AI will differ

    //MotorsController::SetInnerAngle(xAng);
    //MotorsController::SetOuterAngle(yAng);

    //MotorsController::Move();
}

void track()
{
    time = millis();
    ballFound = CameraController::GetBallLocation(xCo, yCo);
    Serial.print(millis() - time); Serial.print(" "); Serial.println("Tracking...");
}
