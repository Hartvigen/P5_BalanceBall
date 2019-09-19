#include "SeanTest.h"

int runSean(void)
{
    /* set pin 5 of PORTB for output*/
    

    return 1;   
}

void runNX()
{
    Serial.begin(9600);
        delay(2000);   

  Serial.print("Starting...\n");
   
    NXShield nxshield;

        Pixy pixy;

    nxshield.init(SH_HardwareI2C);

pixy.init();

   
  uint32_t i=0;
   uint8_t r, g, b;
   
   while(1)
   {
     // calculate r, g, b such that it cycles through the colors
     r = i&0xff;
     g = (i*3)&0xff;
     b = (i/3)&0xff;
     pixy.setLED(r, g, b);
     // We need to delay here because serial requests are handled 
     // every frame period (20ms). If we don't delay, we'll
     // overrun Pixy's receive queue. But that's all OK because 
     // we normally only update the LED once per frame anyway.
     delay(20);
     
     i++;
   }


   
}