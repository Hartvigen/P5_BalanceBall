#include "Sean/SeanTest.h"

int main()
{
    init();
    #if defined(USBCON)
        USBDevice.attach();
    #endif

    runNX();
    runSean();

    return 1;
}