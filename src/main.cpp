#include "headers/Motor.h"

int main()
{
    init();
    #if defined(USBCON)
        USBDevice.attach();
    #endif

    runNX();

    return 0;

}

void runNX()
{
    Serial.begin(9600);  // start serial for output
    delay(500); // wait, allowing time to activate the serial monitor
    int nblobs = 0;  // Number of blobs
    uint8_t bc[8], bl[8], bt[8], br[8], bb[8];
    int x_centre, x_error;
    int y_centre, y_error;
    bool erased = false;
    int i, sq, n, width, height;

    NXShield nxshield;
    nxshield.init(SH_HardwareI2C);

    nxshield.bank_a.motorReset();
    nxshield.bank_b.motorReset();
    
    NXTCam nxcam;
    nxcam.resetCam();
    //Initilize camera with the nxshield, and port Bank A Sensor 1.
    nxcam.init(&nxshield, SH_BAS1);

    //Ved ikk om det gør noget
    nxcam.start(SH_BAS1);
    
    nxcam.enableTracking();
   
    Serial.println ("Rdy");
    //


        while (1)
        {
            //Serial.println(nblobs);

            // Get the blobs from the camera into the array
            nxcam.getBlobs(&nblobs, bc, bl, bt, br, bb);


            // Find the centre of the blob using double resolution of camera
            x_centre = bl[0] + br[0];
            y_centre = bt[0] + bb[0];

            // Compute the error from the desired position of the blob (using double resolution)
            x_error = 176 - x_centre;
            y_error = 143 - y_centre;
           //            Serial.println("ColorMap");

          //  Serial.println(nxcam.getColorMap());

          if (nblobs == 1)
          {
           // Serial.println("nBlobs");
           // Serial.println(nblobs);

            Serial.println("xCenter:");
            Serial.println(x_error);

            Serial.println("yCenter:");
            Serial.println(y_error);  


           // if(-50<x_centre && x_centre<50)
            runMotor(SH_Bank_B,SH_Motor_1,SH_Direction_Forward,10,10);

            delay(1000);

          }
          else{
            runMotor(SH_Bank_A,SH_Motor_1,SH_Direction_Forward,10,10);

            Serial.println("Seaching...");
            Serial.println(nblobs);
            delay(1000);

          }
        }
                

}
