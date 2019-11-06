#include "main.h"
#include "Serial/SerialHelper.h"

int16_t xPrev, yPrev;
int16_t xCo, yCo;
int16_t xAng, yAng;


int main()
{
    initialize();
    setup();

    while(false)
        CameraController::SendImage();
    
    uint16_t i = 0;
    while(true)
    {
        CameraController::GetBallLocation(xCo, yCo);
        Serial.print(i++); Serial.print(" "); Serial.print(xCo); Serial.print(" "); Serial.println(yCo);
    }

    digitalWrite(LED_BUILTIN, HIGH);
    //while (false)
    //    loop();

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
    uint64_t time = millis();
    if (time > trackTime)
    {
        trackTime = time + INTERVAL_TRACK;

        xPrev = xCo;
        yPrev = yCo;

        CameraController::GetBallLocation(xCo, yCo);
        Serial.print("x = "); Serial.print(xCo); Serial.print("  "); Serial.print("y = "); Serial.println(yCo);

        // Get xAng, yAng. This is where PID and AI will differ

        MotorsController::SetInnerAngle(xAng);
        MotorsController::SetOuterAngle(yAng);
    }

    //MotorsController::Move();
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
