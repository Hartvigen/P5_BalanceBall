#include "headers/Motor.h"
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#include "Camera.h"

#define SLAVE_PIN 53

#define SKIP_COUNT 4
#define LEFT_BAR 30
#define RIGHT_BAR 30
#define CAPTURE_RATE 250

typedef struct {
    byte code;
    byte* data;
    uint32_t size;
} Packet;

ArduCAM myCAM(OV2640, SLAVE_PIN);
float Rmin = 31, Gmin = 63, Bmin = 31;

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
    //initNxShield();

    Wire.begin();
    SPI.begin();
    Serial.begin(115200*6); //baudrate
    while (!Serial) {}
    while(Serial.available() > 0)
      Serial.read();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

    float angle = 0;
    float mod = 0.4;
    uint64_t calibrateTime = 0;

    int8_t dir = 0;
    int8_t dirb = 0;

    NXShield shield;
    shield.init(SH_HardwareI2C);
    shield.bank_a.motorReset();
    shield.bank_b.motorReset();


    //shield.bank_a.motorSetSpeedPID(3,1,1);
    //shield.bank_a.motorSetSpeed(SH_Motor_1, 80);
    //shield.bank_a.motorSetSpeed(SH_Motor_2, 80);
    //shield.bank_a.motorSetEncoderPID(12000,1000,30000);
    shield.bank_a.motorStartBothInSync();
    //shield.bank_a.motorSetCommandRegA(SH_Motor_1, 0xB8);

    while (true)
    {
        delay(10);
        angle += mod;
        if (abs(angle) >= 15)
        {
            mod *= -1;
            angle += mod;
        }

        if (millis() > calibrateTime)
        {
            int encoder = shield.bank_a.motorGetEncoderPosition(SH_Motor_Both);
            int8_t adjust = round(angle - encoder);
            float nextDir = adjust > 0 ? 1 : -1;

            if (abs(adjust) <= 1)
            {
                dir = 0;
                shield.bank_a.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
            }
            else if (abs(adjust) <= 3)
            {
                dir = 0;
                shield.bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
            }
            else if (dir != nextDir)
            {
                dir = nextDir;
                shield.bank_a.motorRunUnlimited(
                    SH_Motor_Both, 
                    (adjust > 0 ? SH_Direction_Forward : SH_Direction_Reverse), 
                    5
                );
            }

            int encoderb = shield.bank_b.motorGetEncoderPosition(SH_Motor_Both);
            int8_t adjustb = round(angle - encoderb);
            float nextDirb = adjustb > 0 ? 1 : -1;

            if (abs(adjustb) <= 1)
            {
                dirb = 0;
                shield.bank_b.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
            }
            else if (abs(adjustb) <= 3)
            {
                dirb = 0;
                shield.bank_b.motorStop(SH_Motor_Both, SH_Next_Action_Float);
            }
            else if (dirb != nextDirb)
            {
                dirb = nextDirb;
                shield.bank_b.motorRunUnlimited(
                    SH_Motor_Both, 
                    (adjustb > 0 ? SH_Direction_Forward : SH_Direction_Reverse), 
                    7
                );
            }

            Serial.print("Adjust: ");
            Serial.println(adjust);
            Serial.print("Dir: ");
            Serial.println(dir);
            Serial.print("Angle: ");
            Serial.println(angle);
            Serial.print("Encoder: ");
            Serial.println(encoder);
            Serial.println();

            calibrateTime = millis() + 50;
        }
    }

    /*initCam();
    calibrateCam();
    while(Serial.available() == 0){}
    Serial.read();
    Serial.println("Starting...");
    float xCo, yCo;
    uint64_t nextCapture =  0;
    
    {
        //while(millis() < nextCapture){}
        //nextCapture = millis() + CAPTURE_RATE;
        //xCo = 0, yCo = 0;
        //getBallLocation(xCo, yCo);
        //generalMotorTest();
        

        //getBallLocation
        //Choose action (center of ball, current tilt, velocity of ball)
        //perform action (Set tilt)
    }*/
    return 0;
}

void initCam()
{
    pinMode(SLAVE_PIN, OUTPUT);
    digitalWrite(SLAVE_PIN, HIGH);

    // Resets the CPLD (processor)
    myCAM.write_reg(0x07, 0x80);
    delay(100);
    myCAM.write_reg(0x07, 0x00);
    delay(100);
    
    myCAM.set_format(BMP); // Resolution of 320x240
    myCAM.InitCAM();
    delay(100);

    myCAM.OV2640_set_Special_effects(BW);
}

void calibrateCam(){
    myCAM.flush_fifo();
    myCAM.start_capture();
    while(!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)){}
    
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();

    SPI.transfer(0x00); // The first bit is a header? DON'T remove this. without this, colors goes green goo negative, n' shit.
    for(uint32_t i = 0; i < imageHeight; i++)
    {
        skipImageRow();
        calibrateFromImageRow();;
    }
    
    SPI.endTransaction();
    myCAM.CS_HIGH();
    Serial.println("Finished Calibrating");
}

void getBallLocation(float& xCo, float& yCo)
{
    myCAM.flush_fifo();
    myCAM.start_capture();
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) { }

    readImage(xCo, yCo);
}

void readImage(float& xCo, float& yCo)
{
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();
    uint16_t pointCount = 0;
    SPI.transfer(0x00); // The first bit is a header? DON'T remove this. without this, colors goes green goo negative, n' shit.
    skipImageRow();
    for(uint16_t i = 0; i < imageHeight-1; i += 5)
    {
        skipImageRows();
        readImageRow(pointCount, xCo, yCo, i);
    }
    
    SPI.endTransaction();
    myCAM.CS_HIGH();
}

void readImageRow(uint16_t& pointCount, float& xCo, float& yCo,  uint16_t  rowNumber)
{
    for (uint16_t i = 0; i < imageWidth; i++)
    {
        byte b1 = SPI.transfer(0x00);
        delayMicroseconds(1);
        byte b2 = SPI.transfer(0x00);

        if(i < LEFT_BAR || i > imageWidth-RIGHT_BAR)
            continue;
        uint32_t c565 = b1 | b2 << 8;

        if((c565 & 0x1f) < Rmin && ((c565 >> 5) & 0x3f) < Gmin && ((c565 >> 11) & 0x1f) < Bmin){
            pointCount += 1;
            xCo += ((i - ceil((float) imageWidth/2)) - xCo) / pointCount;
            yCo += (rowNumber - ceil((float) imageHeight/2) - yCo)/pointCount;
        }

    }
}

void calibrateFromImageRow()
{
    for (uint32_t i = 0; i < imageWidth; i++)
    {
        byte b1 = SPI.transfer(0x00);
        delayMicroseconds(1);
        byte b2 = SPI.transfer(0x00);
        if(i < LEFT_BAR || i > imageWidth-RIGHT_BAR)
            continue;

        uint32_t c565 = b1 | b2 << 8;

        Rmin = min(Rmin, (c565 & 0x1f));   
        Gmin = min(Gmin, ((c565 >> 5) & 0x3f));
        Bmin = min(Bmin,  ((c565 >> 11) & 0x1f));
    }
}

void skipImageRows()
{
    uint32_t skipcount = SKIP_COUNT * imageWidth;
    for (uint32_t i = 0; i < skipcount; i++)
    {
        SPI.transfer(0x00);
        SPI.transfer(0x00);
    }
}

void skipImageRow()
{
    for (uint32_t i = 0; i < imageWidth; i++)
    {
        SPI.transfer(0x00);
        SPI.transfer(0x00);
    }
}

