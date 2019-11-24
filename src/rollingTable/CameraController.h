#ifndef CAMERA_CONTROLLER_H_INCLUDE
#define CAMERA_CONTROLLER_H_INCLUDE

#include "Setup.h"

#include <stdint.h>
#include <Arduino.h>
#include <ArduCAM.h>
#include <SPI.h>

#if USE_IMG_DIS
#include "Serial/SerialHelper.h"
#endif

#define CAPTURE_WIDTH (uint16_t)160
#define CAPTURE_HEIGHT (uint16_t)120

#define LEFT_MARGIN (uint16_t)16
#define RIGHT_MARGIN (uint16_t)15
#define TOP_MARGIN (uint16_t)1

#define IMAGE_WIDTH (CAPTURE_WIDTH - LEFT_MARGIN - RIGHT_MARGIN)
#define IMAGE_HEIGHT (CAPTURE_HEIGHT - TOP_MARGIN)

#define ROW_SKIP_COUNT (uint8_t)2
#define MAX_AVERAGED_POINTS (uint8_t)20


namespace RollingTable
{
    class CameraController 
    {
        private:
            static ArduCAM camera;
            static uint8_t limitR, limitG, limitB;
            
            static void SkipRows(uint16_t count);
            static void SkipColumns(uint16_t count);

            static void BeginRead();
            static void EndRead();

        public:
            static void Init(int slavePin, uint8_t r, uint8_t g, uint8_t b);
            
            static void BeginCapture();
            static void StartTracking();
            static void ProceedTracking(uint16_t trackTimes);
            static bool EndTracking(int16_t& xCo, int16_t& yCo);

#if USE_IMG_DIS
            static void SendImageToProcessing();
#endif
    };
}

#endif