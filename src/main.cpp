#include "headers/Motor.h"
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#include "headers/Camera.h"

#define SLAVE_PIN 53
#define SKIP_COUNT 4
#define LEFT_BAR 30
#define RIGHT_BAR 30
#define CAPTURE_RATE 250

//Needed for main 
void initCam();
void calibrateCam();
void calibrateFromImageRow();
void getBallLocation(float& xCo, float& yCo);
void readImage(float& xCo, float& yCo);
void readImageRow(uint16_t& pointCount, float& xCo, float& yCo, uint16_t  rowNumber);
void skipImageRows();
void skipImageRow();
void MotorTest1();


const uint32_t imageWidth = 320;
const uint32_t imageHeight = 240;

int main()
{
    init();
    initNxShield();

    Wire.begin();
    SPI.begin();
    Serial.begin(115200*6); //baudrate
    while (!Serial) {}
    while(Serial.available() > 0)
      Serial.read();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
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
    return 0;
}

