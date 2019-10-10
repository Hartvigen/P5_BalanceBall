#include "Sean/SeanTest.h"

int main()
{
    init();
    #if defined(USBCON)
        USBDevice.attach();
    #endif

    Serial.begin(9600);

    runSean();

    return 1;
}