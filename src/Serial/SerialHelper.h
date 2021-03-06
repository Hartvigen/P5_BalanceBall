#include "Setup.h"
#if USE_IMG_DIS

#ifndef SERIAL_HELPER_H_INCLUDE
#define SERIAL_HELPER_H_INCLUDE

#include <stdint.h>
#include <Arduino.h>

class SerialHelper
{
    public:
        static void AwaitSignal();
        static void SendBytes(uint8_t* arr, uint32_t size);
        static void SendInt(int32_t value);
};

#endif

#endif // USE_IMG_DIS