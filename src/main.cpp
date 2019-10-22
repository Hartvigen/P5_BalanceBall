#ifndef UNIT_TEST
#include "headers/Motor.h"
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

#define SLAVE_PIN 53

typedef struct {
    byte code;
    byte* data;
    uint32_t size;
} Packet;

ArduCAM myCAM(OV2640, SLAVE_PIN);

void initCam();
void takeAndWritePicture();

void readAndSendImage(ArduCAM myCAM);
byte* readImageRow(ArduCAM myCAM);
void skipImageRow(ArduCAM myCAM);

void writeSerial(byte code, byte* bytes, uint32_t length);
Packet readSerial();
byte* intToBytes(uint32_t value);

const uint32_t imageWidth = 320;
const uint32_t imageHeight = 240;


int main()
{
    init();
    initNxShield();
    generalMotorTest();
/*
    Wire.begin();
    SPI.begin();
    Serial.begin(115200*6);
    while (!Serial) {}
    while(Serial.available() > 0)
        Serial.read();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

    initCam();
    while (true)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        while (!Serial.available()) {}
        Serial.read();
        digitalWrite(LED_BUILTIN, LOW);

        takeAndWritePicture();

        while (Serial.available()) Serial.read();
    }

    return 1;*/
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

    myCAM.OV2640_set_Brightness(Brightness2);
    myCAM.OV2640_set_Special_effects(BW);
}

void takeAndWritePicture()
{
    myCAM.flush_fifo();
    myCAM.start_capture();
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) { }

    readAndSendImage(myCAM);
}

void readAndSendImage(ArduCAM myCAM)
{
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();

    SPI.transfer(0x00); // The first bit is a header? DON'T remove this. without this, colors goes green goo negative, n' shit.
    for(uint32_t i = 0; i < imageHeight; i++)
    {
        byte* row = readImageRow(myCAM);
        writeSerial(0x01, row, imageWidth*3);
        while(!Serial.available()) {}
        Serial.read();
        delete row;
    }
    
    SPI.endTransaction();
    myCAM.CS_HIGH();
}

byte* readImageRow(ArduCAM myCAM)
{
    byte* row = (byte*)malloc(imageWidth*3);

    for (uint32_t i = 0; i < imageWidth; i++)
    {
        byte b1 = SPI.transfer(0x00);
        delayMicroseconds(1);
        byte b2 = SPI.transfer(0x00);

        uint32_t c565 = b1 | b2 << 8;

        row[3*i+2] = (c565 & 0x1f);
        row[3*i+1] = ((c565 >> 5) & 0x3f);
        row[3*i]   = ((c565 >> 11) & 0x1f);
    }

    return row;
}

void skipImageRow(ArduCAM myCAM)
{
    for (uint32_t i = 0; i < imageWidth; i++)
    {
        SPI.transfer(0x00);
        SPI.transfer(0x00);
    }
}

void writeSerial(byte code, byte* bytes, uint32_t length)
{
    byte* size = intToBytes(length);
    Serial.write(code);
    Serial.write(size, 4);
    Serial.write(bytes, length);
    Serial.flush();
    delete size;
}

Packet readSerial()
{
    Packet pac;

    byte header[5];
    while(Serial.available() < 5) { delay(2); }
    Serial.readBytes(header, 5);
    
    pac.code = header[0];
    pac.size = header[1] | header[2] << 8 | ((uint32_t)header[3]) << 16 | ((uint32_t)header[4]) << 24;
    
    uint32_t size = pac.size;
    pac.data = (byte*)malloc(size);

    while((uint32_t)Serial.available() < (uint32_t)size) { delay(2); }
    Serial.readBytes(pac.data, size);

    while (Serial.available() > 0)
        Serial.read();

    return pac;
}

byte* intToBytes(uint32_t value)
{
    byte* buf = (byte*)malloc(4);

    buf[0] = value & 255;
    buf[1] = (value >> 8)  & 255;
    buf[2] = (value >> 16) & 255;
    buf[3] = (value >> 24) & 255;

    return buf;
}

#endif