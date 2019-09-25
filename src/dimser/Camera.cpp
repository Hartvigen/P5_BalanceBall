#include "headers/Camera.h"

// Initilize the camera, and returns the object.
NXTCam startAndReturnCamera(NXShield nxshield){
    NXTCam nxcam;
    nxcam.resetCam();
    //Initilize camera with the nxshield, and port Bank A Sensor 1.
    nxcam.init(&nxshield, SH_BAS1);

    //Ved ikk om det gør noget
    nxcam.start(SH_BAS1);
    
    nxcam.enableTracking();

    return nxcam;
}


//Gets all blobs, and returns the blobs, and x and y coordinates though pointers.
void getAllBlobs(NXTCam nxcam, int* ptrNumberBlobs, int* ptrX_error, int* ptrY_error){
    ptrNumberBlobs = 0;  // Number of blobs
    uint8_t *bc[8], *bl[8], *bt[8], *br[8], *bb[8];
    int x_centre;
    int y_centre;
    int width, height;
        Serial.println(*ptrNumberBlobs);

    nxcam.getBlobs(ptrNumberBlobs, *bc, *bl, *bt, *br, *bb);
    Serial.println(*ptrNumberBlobs);

    x_centre = *bl[0] + *br[0];
    y_centre = *bt[0] + *bb[0];

    // Compute the error from the desired position of the blob (using double resolution)
    *ptrX_error = 176 - x_centre;
    *ptrY_error = 143 - y_centre;

}

