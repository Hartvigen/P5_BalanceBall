
#include <Wire.h>
#include <BreezyArduCAM.h>
#include <SPI.h>
//#include "headers/Motor.h"

int main()
{
    init();

    Serial_ArduCAM_FrameGrabber fg;
    ArduCAM_Mini_2MP myCam(53, &fg); //53 referes to the slave pin number.

    // ArduCAM Mini uses both I^2C and SPI buses
    Wire.begin();
    SPI.begin();

    // Fastest baud rat
    Serial.begin(921600);

    // Start the camera in JPEG mode with a specific image size
    myCam.beginJpeg320x240();


    while (1)
    {
      myCam.capture();
    }
    
    return 0;
}
