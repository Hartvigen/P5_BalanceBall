#include "headers/Camera.h"
#include "headers/Motor.h"

int main()
{
    init();
    
    runMoterTest();

    runCameraTest();
    
    return 0;
}

void runMoterTest(){
    
    runMotor(SH_Bank_B,SH_Motor_1,SH_Direction_Forward,10,100);
    
}

void runCameraTest(){
    Arducam myCam = initCam();

    // Fastest baud rat
    Serial.begin(921600);

    // Start the camera in JPEG mode with a specific image size
    myCam.beginJpeg320x240();


    while (1)
    {
      myCam.capture();
    }
}
