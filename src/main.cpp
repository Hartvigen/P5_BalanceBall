#include "Sean/SeanTest.h"

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

void writeSerial(byte code, uint32_t value);
void writeSerial(byte code, byte* bytes, uint32_t length);
Packet readSerial();
byte* intToBytes(uint32_t value);

void read_fifo_burst(ArduCAM myCAM);
void readAndSendImage(ArduCAM myCAM);
void takeAndWritePicture();
void initCam();

int shift = 0;
ArduCAM myCAM(OV2640, SLAVE_PIN);


int main()
{
    init();
    Wire.begin();
    SPI.begin();
    Serial.begin(921600);
    while (!Serial) {}

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    pinMode(SLAVE_PIN, OUTPUT);
    digitalWrite(SLAVE_PIN, HIGH);
    
    /*
    initCam();
    while (true)
    {
        // Read serial input here
        takeAndWritePicture();
    }
    */

    delay(1000);
    Packet pac = readSerial();
    writeSerial(1, pac.data, pac.size);

    return 1;
}

void initCam()
{
    // Resets the CPLD (processor)
    myCAM.write_reg(0x07, 0x80);
    delay(100);
    myCAM.write_reg(0x07, 0x00);
    delay(100);
    
    myCAM.set_format(BMP); // Resolution of 320x240
    myCAM.InitCAM();
    //delay(500);
}

void takeAndWritePicture()
{
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    myCAM.start_capture();

    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) { }
    read_fifo_burst(myCAM);
}

void readAndSendImage(ArduCAM myCAM)
{
    
}

void read_fifo_burst(ArduCAM myCAM)
{
    uint32_t length = myCAM.read_fifo_length();
    writeSerial(1, length);
    while (Serial.available() == 0) {}
    Serial.read();

    int size = 320 * 2; // resolution width * 2 (rgb)
    byte data[size];
    int i = 0;

    myCAM.CS_LOW();
    myCAM.set_fifo_burst(); //Set fifo burst mode

    //shift += 10;
    //for (int x = 0; x < shift; x++)
    //    SPI.transfer(0x00);

    while (length)
    {
        i = 0;
        while (length && i < size)
        {
            length--;
            data[i++] = SPI.transfer(0x00);
            delayMicroseconds(15);
        }

        if (i > 0)
            Serial.write(data, i);
    }
    
    myCAM.CS_HIGH();
}

void writeSerial(byte code, uint32_t value)
{
    byte* buf = intToBytes(value);
    writeSerial(code, buf, 4);
    delete buf;
}

void writeSerial(byte code, byte* bytes, uint32_t length)
{
    digitalWrite(LED_BUILTIN, HIGH); 
    byte* size = intToBytes(length);
    Serial.write(code);
    Serial.write(size, 4);
    Serial.write(bytes, length);
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
    
    while(Serial.available() < (int)size) { delay(2); }

    Serial.readBytes(pac.data, size);

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