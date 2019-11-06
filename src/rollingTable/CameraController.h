#ifndef CAMERA_CONTROLLER_H_INCLUDE
#define CAMERA_CONTROLLER_H_INCLUDE

#include <stdint.h>
#include <Arduino.h>
#include <ArduCAM.h>
#include <SPI.h>

#define CAPTURE_WIDTH (uint16_t)320
#define CAPTURE_HEIGHT (uint16_t)240

#define LEFT_MARGIN (uint16_t)16
#define RIGHT_MARGIN (uint16_t)15
#define TOP_MARGIN (uint16_t)1

#define IMAGE_WIDTH (CAPTURE_WIDTH - LEFT_MARGIN - RIGHT_MARGIN)
#define IMAGE_HEIGHT (CAPTURE_HEIGHT - TOP_MARGIN)

#define SKIP_COUNT (uint16_t)3


namespace RollingTable
{
    class CameraController 
    {
        private:
            static ArduCAM camera;
            static uint8_t minR, minG, minB;

            static void Capture();
            static void SkipRows(uint16_t count);
            static void SkipColumns(uint16_t count);

            static void BeginRead();
            static void EndRead();

        public:
            static void Init(int slavePin);

            static void Recalibrate();
            static void GetBallLocation(int16_t& xCo, int16_t& yCo);
            static void SendImage();
    };
}

#endif