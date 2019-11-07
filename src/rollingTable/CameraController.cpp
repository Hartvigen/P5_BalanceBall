#include "CameraController.h"
#include "Serial/SerialHelper.h"

namespace RollingTable
{
    ArduCAM CameraController::camera;
    uint8_t CameraController::minR, CameraController::minG, CameraController::minB;

    float xAvgPart, yAvgPart;
    uint32_t pointsAveragedPart;
    uint16_t rowPart;


    void CameraController::Init(int slavePin)
    {
        minR = minG = minB = 0;
        camera = ArduCAM(OV2640, slavePin);

        pinMode(slavePin, OUTPUT);
        digitalWrite(slavePin, HIGH);
        
        // Resets the CPLD (processor)
        camera.write_reg(0x07, 0x80); delay(100);
        camera.write_reg(0x07, 0x00); delay(100);
        
        camera.set_format(BMP); 
        camera.InitCAM();
        delay(100);

        camera.OV2640_set_Special_effects(BW);
    }


    void CameraController::Capture()
    {
        camera.flush_fifo();
        camera.start_capture();
        while (!camera.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) { }

        // Skip dummy byte here
        camera.read_fifo();
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


    void CameraController::Recalibrate()
    {
        Capture();
        BeginRead();

        minR = minB = 31;
        minG = 63;

        SkipRows(TOP_MARGIN);
        for (uint16_t row = 0; row < IMAGE_HEIGHT; row++)
        {
            SkipColumns(LEFT_MARGIN);

            for (uint16_t col = 0; col < IMAGE_WIDTH; col++)
            {
                uint16_t c565 = SPI.transfer16(0x00);

                uint8_t R = (c565 & 0x1f);
                uint8_t G = ((c565 >> 5) & 0x3f);
                uint8_t B = ((c565 >> 11) & 0x1f);

                if (R > 5 && R < minR) minR = R;
                if (G > 10 && G < minG) minG = G;
                if (B > 5 && B < minB) minB = B;
            }

            SkipColumns(RIGHT_MARGIN);
        }

        EndRead();

        ///*
        Serial.print("minR: "); Serial.print(minR); Serial.print("  ");
        Serial.print("minG: "); Serial.print(minG); Serial.print("  ");
        Serial.print("minB: "); Serial.print(minB); Serial.println();
        //*/

        /*
        Serial.write(minR);
        SerialHelper::AwaitSignal();
        Serial.write(minG);
        SerialHelper::AwaitSignal();
        Serial.write(minB);
        //*/
    }


    bool CameraController::GetBallLocation(int16_t& xCo, int16_t& yCo)
    {
        uint32_t pointsAveraged = 0; // Is uint32 since 240*320 > UINT16_MAX
        float avgX = 0, avgY = 0;

        Capture();
        BeginRead();

        SkipRows(TOP_MARGIN);
        for (uint16_t row = 0; row < IMAGE_HEIGHT; row += (FULL_SKIP_COUNT+1))
        {
            SkipColumns(LEFT_MARGIN);

            for (uint16_t col = 0; col < IMAGE_WIDTH; col++)
            {
                uint16_t c565 = SPI.transfer16(0x00);
                
                if (col % (FULL_SKIP_COUNT+1) == 0)
                {
                    if ((c565 & 0x1f) < minR && ((c565 >> 5) & 0x3f) < minG && ((c565 >> 11) & 0x1f) < minB)
                    {
                        pointsAveraged += 1;
                        avgX += (col - avgX) / pointsAveraged;
                        avgY += (row - avgY) / pointsAveraged;
                    }
                }
            }

            SkipColumns(RIGHT_MARGIN);
            SkipRows(FULL_SKIP_COUNT);
        }
        
        EndRead();

        // Return coordinates offset to have center in (0,0)
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


    void CameraController::SendImage()
    {
        uint32_t pointsAveraged = 0; // Is uint32 since 240*320 > UINT16_MAX
        float avgX = 0, avgY = 0;

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

                bytes[col*3+0] = (c565 & 0x1f);         // R
                bytes[col*3+1] = ((c565 >> 5) & 0x3f);  // G
                bytes[col*3+2] = ((c565 >> 11) & 0x1f); // B

                if (row % (FULL_SKIP_COUNT+1) == 0 && 
                    bytes[col*3+0] < minR && (bytes[col*3+1] & 0x3f) < minG && (bytes[col*3+2] & 0x1f) < minB)
                {
                    bytes[col*3+0] = 31;
                    bytes[col*3+1] = 0;
                    bytes[col*3+2] = 0;

                    pointsAveraged += 1;
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


    void CameraController::StartTracking()
    {
        xAvgPart = 0;
        yAvgPart = 0;
        rowPart = 0;
        pointsAveragedPart = 0;
        
        Capture();

        BeginRead();
        SkipRows(TOP_MARGIN);
        EndRead();
    }

    void CameraController::ProceedTracking(uint16_t trackTimes)
    {
        BeginRead();
        
        for (uint16_t row = 0; row < trackTimes; row++)
        {
            SkipColumns(LEFT_MARGIN);
            for (uint16_t col = 0; col < IMAGE_WIDTH; col++)
            {
                uint16_t c565 = SPI.transfer16(0x00);
                
                if (col % (PART_SKIP_COUNT+1) == 0)
                {
                    if ((c565 & 0x1f) < minR && ((c565 >> 5) & 0x3f) < minG && ((c565 >> 11) & 0x1f) < minB)
                    {
                        pointsAveragedPart += 1;
                        xAvgPart += (col - xAvgPart) / pointsAveragedPart;
                        yAvgPart += (rowPart - yAvgPart) / pointsAveragedPart;
                    }
                }
            }
            SkipColumns(RIGHT_MARGIN);
            SkipRows(PART_SKIP_COUNT);

            rowPart += (PART_SKIP_COUNT+1);
        }

        EndRead();
    }

    bool CameraController::EndTracking(int16_t& xCo, int16_t& yCo)
    {
        if (pointsAveragedPart != 0)
        {
            xCo = (int16_t)round(xAvgPart) - (int16_t)(IMAGE_WIDTH/2);
            yCo = (int16_t)round(yAvgPart) - (int16_t)(IMAGE_HEIGHT/2);
            return true;
        }
        else
        {
            xCo = yCo = 0;
            return false;
        }
    }
}