#if USE_IMG_DIS
#include "SerialHelper.h"

void SerialHelper::AwaitSignal()
{
    while (!Serial.available()) { delay(2); }
        Serial.read();
}

void SerialHelper::SendBytes(uint8_t* arr, uint32_t size)
{
    SendInt((int32_t)size);
    Serial.write(arr, size);
}

void SerialHelper::SendInt(int32_t value)
{
    uint8_t bytes[] = { 
        (uint8_t)(value),
        (uint8_t)(value >> 8),
        (uint8_t)(value >> 16),
        (uint8_t)(value >> 24)
    };

    Serial.write(bytes, 4);
}
#endif