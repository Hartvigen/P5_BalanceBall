#ifndef CAMERA_CONTROLLER_H_INCLUDE
#define CAMERA_CONTROLLER_H_INCLUDE

#include <stdint.h>
#include <Arduino.h>
#include <ArduCAM.h>
#include <SPI.h>

#define CAPTURE_WIDTH (uint16_t)320
#define CAPTURE_HEIGHT (uint16_t)240

#define LEFT_MARGIN (uint16_t)0
#define RIGHT_MARGIN (uint16_t)0
#define TOP_MARGIN (uint16_t)0

#define IMAGE_WIDTH (CAPTURE_WIDTH - LEFT_MARGIN - RIGHT_MARGIN)
#define IMAGE_HEIGHT (CAPTURE_HEIGHT - TOP_MARGIN)

#define SKIP_COUNT (uint16_t)4


namespace RollingTable
{
    class CameraController 
    {
        private:
            ArduCAM camera;
            uint8_t minR, minG, minB;

            void Capture();
            void SkipRows(uint16_t count);
            void SkipColumns(uint16_t count);

            void BeginRead();
            void EndRead();

        public:
            CameraController();
            CameraController(int slavePin);

            void Recalibrate();
            void GetBallLocation(int16_t& xCo, int16_t& yCo);
    };
}

#endif