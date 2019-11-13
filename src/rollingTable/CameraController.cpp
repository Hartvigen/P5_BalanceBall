#include "CameraController.h"

namespace RollingTable
{
    ArduCAM CameraController::camera;
    uint8_t CameraController::minR;
    uint8_t CameraController::minG;
    uint8_t CameraController::minB;

    uint32_t pointsAveraged; // Is uint32 since 240*320 > UINT16_MAX
    float avgX, avgY;


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
        SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

        camera.flush_fifo();
        camera.start_capture();
        
        camera.CS_LOW();
        do SPI.transfer(ARDUCHIP_TRIG); 
        while (!(SPI.transfer(0x00) & CAP_DONE_MASK));
        camera.CS_HIGH();
        
        // Skip dummy byte here
        camera.read_fifo();
        
        SPI.endTransaction();
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


    void CameraController::AutoCalibrate()
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

                uint8_t R = (c565 & 0x1F);
                uint8_t G = ((c565 >> 5) & 0x3F);
                uint8_t B = ((c565 >> 11) & 0x1F);

                if ((R > 4 && G > 8 && B > 4) && (R < minR && G < minG && B < minB))
                {
                    minR = R;
                    minG = G;
                    minB = B;
                }
            }

            SkipColumns(RIGHT_MARGIN);
        }

        EndRead();

        minR--; minG--; minB--;

#if USE_IMG_DIS
        Serial.write(minR);
        SerialHelper::AwaitSignal();
        Serial.write(minG);
        SerialHelper::AwaitSignal();
        Serial.write(minB);
#else
        Serial.print("minR: "); Serial.print(minR); Serial.print("  ");
        Serial.print("minG: "); Serial.print(minG); Serial.print("  ");
        Serial.print("minB: "); Serial.print(minB); Serial.println();
#endif  
    }

    void CameraController::ManualCalibrate(uint8_t r, uint8_t g, uint8_t b)
    {
        minR = r;
        minG = g;
        minB = b;
    }


    bool CameraController::GetBallLocation(int16_t& xCo, int16_t& yCo)
    {
        pointsAveraged = 0;
        avgX = avgY = 0;

        Capture();
        BeginRead();

        SkipRows(TOP_MARGIN);
        for (uint16_t row = 0; row < IMAGE_HEIGHT; row += (ROW_SKIP_COUNT+1))
        {
            SkipColumns(LEFT_MARGIN);
            for (uint16_t col = 0; col < IMAGE_WIDTH; col++)
            {
                uint16_t c565 = SPI.transfer16(0x00);

                if ((c565 & 0x1F) < minR && ((c565 >> 5) & 0x3F) < minG && ((c565 >> 11) & 0x1F) < minB)
                {
                    ++pointsAveraged;
                    avgX += (col - avgX) / pointsAveraged;
                    avgY += (row - avgY) / pointsAveraged;
                }
            }

            SkipColumns(RIGHT_MARGIN);
            SkipRows(ROW_SKIP_COUNT);
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