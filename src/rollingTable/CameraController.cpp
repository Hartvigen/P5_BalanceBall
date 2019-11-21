#include "CameraController.h"

namespace RollingTable
{
    ArduCAM CameraController::camera;
    uint8_t CameraController::minR;
    uint8_t CameraController::minG;
    uint8_t CameraController::minB;

    uint8_t currentRow;
    uint16_t pointsAveraged;
    float avgX, avgY;


    void CameraController::Init(int slavePin, uint8_t r, uint8_t g, uint8_t b)
    {
        camera = ArduCAM(OV2640, slavePin);
        pinMode(slavePin, OUTPUT);
        digitalWrite(slavePin, HIGH);
        
        // Resets the camera's processor
        camera.write_reg(0x07, 0x80); delay(100);
        camera.write_reg(0x07, 0x00); delay(100);
        
        camera.set_format(BMP); 
        camera.InitCAM();
        delay(100);

        camera.OV2640_set_Special_effects(BW);
        
        minR = r;
        minG = g;
        minB = b;
    }


    void CameraController::SkipRows(uint16_t count)
    {
        for (uint16_t row = 0; row < count; row++)
            for (uint16_t col = 0; col < CAPTURE_WIDTH; col++)
                SPI.transfer16(0x00);
    }

    void CameraController::SkipColumns(uint16_t count)
    {
        for (uint16_t col = 0; col < count; col++)
            SPI.transfer16(0x00);
    }


    void CameraController::BeginRead()
    {
        // The value of 8_000_000 MUST NOT be exceeded due to ArduCAM hardware
        SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
        camera.CS_LOW();
        camera.set_fifo_burst();
    }

    void CameraController::EndRead()
    {
        camera.CS_HIGH();
        SPI.endTransaction();
    }


    void CameraController::BeginCapture()
    {
        camera.flush_fifo();
        camera.start_capture();
    }

    void CameraController::StartTracking()
    {
        pointsAveraged = 0;
        avgX = avgY = 0;
        currentRow = 0;

        BeginRead();
        SPI.transfer(0x00); // Skip dummy byte
        SkipRows(TOP_MARGIN);
        EndRead();
    }

    void CameraController::ProceedTracking(uint16_t trackTimes)
    {
        if (pointsAveraged > MAX_AVERAGED_POINTS)
            return;

        BeginRead();
        for (uint16_t row = 0; row < trackTimes; row++)
        {
            SkipColumns(LEFT_MARGIN);
            for (uint16_t col = 0; col < IMAGE_WIDTH; col++)
            {
                uint16_t c565 = SPI.transfer16(0x00);

                if ((c565 & 0x1F) < minR && ((c565 >> 5) & 0x3F) < minG && ((c565 >> 11) & 0x1F) < minB)
                {
                    if (++pointsAveraged > MAX_AVERAGED_POINTS)
                        return;
                    
                    avgX += (col - avgX) / pointsAveraged;
                    avgY += (currentRow - avgY) / pointsAveraged;
                }
            }
            SkipColumns(RIGHT_MARGIN);
            SkipRows(ROW_SKIP_COUNT);

            currentRow += (ROW_SKIP_COUNT+1);
        }
        EndRead();
    }

    bool CameraController::EndTracking(int16_t& xCo, int16_t& yCo)
    {
        if (pointsAveraged != 0)
        {
            xCo = (int16_t)round(avgX) - (int16_t)(IMAGE_WIDTH/2);
            yCo = (int16_t)round(avgY) - (int16_t)(IMAGE_HEIGHT/2);
            return true;
        }
        else
        {
            xCo = yCo = 0;
            return false;
        }
    }


#if USE_IMG_DIS
    void CameraController::SendImageToProcessing()
    {
        pointsAveraged = 0;
        avgX = avgY = 0;

        uint8_t* bytes = (uint8_t*)malloc(IMAGE_WIDTH * 3);

        SerialHelper::AwaitSignal();
        SerialHelper::SendInt(IMAGE_HEIGHT);
        SerialHelper::SendInt(IMAGE_WIDTH);
        
        Capture();
        BeginRead();

        SkipRows(TOP_MARGIN);
        for (uint16_t row = 0; row < IMAGE_HEIGHT; row++)
        {
            SkipColumns(LEFT_MARGIN);

            for (uint16_t col = 0; col < IMAGE_WIDTH; col++)
            {
                uint16_t c565 = SPI.transfer16(0x00);

                bytes[col*3+0] = (c565 & 0x1F);         // R
                bytes[col*3+1] = ((c565 >> 5) & 0x3F);  // G
                bytes[col*3+2] = ((c565 >> 11) & 0x1F); // B

                if (row % (ROW_SKIP_COUNT+1) == 0 &&
                    bytes[col*3+0] < minR && bytes[col*3+1] < minG && bytes[col*3+2] < minB)
                {
                    bytes[col*3+0] = 31;
                    bytes[col*3+1] = 0;
                    bytes[col*3+2] = 0;

                    ++pointsAveraged;
                    avgX += (col - avgX) / pointsAveraged;
                    avgY += (row - avgY) / pointsAveraged;
                }
            }

            SkipColumns(RIGHT_MARGIN);

            SerialHelper::AwaitSignal();
            SerialHelper::SendBytes(bytes, IMAGE_WIDTH * 3);
        }

        EndRead();

        delete bytes;

        SerialHelper::AwaitSignal();
        SerialHelper::SendInt(pointsAveraged == 0 ? 0 : ((int16_t)round(avgX) - (int16_t)(IMAGE_WIDTH/2)));
        SerialHelper::AwaitSignal();
        SerialHelper::SendInt(pointsAveraged == 0 ? 0 : ((int16_t)round(avgY) - (int16_t)(IMAGE_HEIGHT/2)));
    }
#endif
}