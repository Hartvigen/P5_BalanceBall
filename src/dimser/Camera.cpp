#include <ArduCAM.h>
#include <SPI.h>
#include <Arduino.h>
#include "headers/Camera.h"

#define SLAVE_PIN 53
#define SKIP_COUNT 4
#define LEFT_BAR 30
#define RIGHT_BAR 30
#define CAPTURE_RATE 250

typedef struct 
{
    byte code;
    byte* data;
    uint32_t size;
} 
Packet;

ArduCAM myCAM(OV2640, SLAVE_PIN);
float Rmin = 31, Gmin = 63, Bmin = 31;

const uint32_t imageWidth = 320;
const uint32_t imageHeight = 240;


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


void calibrateCam()
{
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
