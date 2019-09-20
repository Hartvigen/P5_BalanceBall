#include "SeanTest.h"
int runSean(void)
{
    /* set pin 5 of PORTB for output*/
    

    return 1;   
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

    
    NXTCam nxcam;

    //Initilize camera with the nxshield, and port Bank A Sensor 1.
    nxcam.init(&nxshield, SH_BAS1);

    //Ved ikk om det gør noget
    nxcam.start(SH_BAS1);

    Serial.println ("Rdy");
    //


        while (1)
        {
            Serial.println(nblobs);

            // Get the blobs from the camera into the array
            nxcam.getBlobs(&nblobs, bc, bl, bt, br, bb);


            // Find the centre of the blob using double resolution of camera
            x_centre = bl[0] + br[0];
            y_centre = bt[0] + bb[0];

            // Compute the error from the desired position of the blob (using double resolution)
            x_error = 176 - x_centre;
            y_error = 143 - y_centre;

            Serial.println(nblobs);
            Serial.println(x_centre);
            Serial.println(y_centre);
        
    }
}