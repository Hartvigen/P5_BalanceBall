#include "headers/Camera.h"

// Initilize the camera, and returns the object.
Arducam initCam(){

    Serial_ArduCAM_FrameGrabber fg;
    Arducam myCam(53, &fg); //53 referes to the slave pin number.

    // ArduCAM Mini uses both I^2C and SPI buses
    Wire.begin();
    SPI.begin();


    return myCam;
}