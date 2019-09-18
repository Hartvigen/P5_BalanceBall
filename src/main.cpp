#include "Sean/SeanTest.h"

int main()
{
    init();
    #if defined(USBCON)
        USBDevice.attach();
    #endif

    runNX();

    return 1;
}