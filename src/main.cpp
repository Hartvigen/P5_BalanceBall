#include "headers/Camera.h"
#include "headers/Motor.h"

int main()
{
    init();

    Serial.begin(9600); // start serial for output
    delay(1000); // wait, allowing time to activate the serial monitor
    Serial.println("Rdy");

    NXShield nxshield;
    nxshield.init(SH_HardwareI2C);

    runNX(nxshield);

    return 0;
}

void runNX(NXShield nxshield)
{
    int* ptrNumberBlobs = 0;  // Number of blobs
    int* ptrX_error = 0; // x coordinates, fixed to 0 being center.
    int* ptrY_error = 0; // y coordinates, fixed to 0 being center.
    
    NXTCam nxcam = startAndReturnCamera(nxshield); 

        while (1)
        {

        getAllBlobs(nxcam, ptrNumberBlobs, ptrX_error, ptrY_error);

          if (*ptrNumberBlobs == 1)
          {

            Serial.println("xCenter:");
            //Serial.println(*ptrX_error);

            Serial.println("yCenter:");
           // Serial.println(*ptrY_error);  


           // if(-50<x_centre && x_centre<50)
            runMotor(SH_Bank_B,SH_Motor_1,SH_Direction_Forward,10,10);

            delay(1000);

          }
          else{
            //runMotor(SH_Bank_A,SH_Motor_1,SH_Direction_Forward,10,10);

            Serial.println("Seaching...");
            Serial.println(*ptrNumberBlobs);
            delay(1000);

          }
        }           
}
