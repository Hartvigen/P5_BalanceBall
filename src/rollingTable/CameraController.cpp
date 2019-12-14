#include "CameraController.h"

namespace RollingTable
{
    ArduCAM CameraController::camera;
    uint8_t CameraController::limitR;
    uint8_t CameraController::limitG;
    uint8_t CameraController::limitB;

    uint8_t currentRow;
    uint16_t pointsAveraged;
    float avgX, avgY;


    void CameraController::Init(int slavePin, uint8_t r, uint8_t g, uint8_t b)
    {
        //Lines 18 to 26 are standard for the ArduCam, and taken from examples thereof.
        camera = ArduCAM(OV2640, slavePin);

        // Resets the camera's processor
        camera.write_reg(0x07, 0x80);
        delay(100);
        camera.write_reg(0x07, 0x00);
        delay(100);

        camera.set_format(BMP);
        camera.InitCAM();
        delay(100);

        camera.OV2640_set_Special_effects(BW);

        //Set the colour limits for tracking.
        limitR = r;
        limitG = g;
        limitB = b;
    }


    //Function skips 'count' rows.
    inline void CameraController::SkipRows(uint16_t count)
    {
        for (uint16_t row = 0; row < count; row++)
            for (uint16_t col = 0; col < CAPTURE_WIDTH; col++)
                SPI.transfer16(0x00);
    }

    //Functions skips 'count' pixels in current row.
    inline void CameraController::SkipColumns(uint16_t count)
    {
        for (uint16_t col = 0; col < count; col++)
            SPI.transfer16(0x00);
    }


    //Prepares SPI connection and camera for transmission of pixel bytes.
    inline void CameraController::BeginRead()
    {
        // The value of 8_000_000 MUST NOT be exceeded due to ArduCAM hardware
        SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
        camera.CS_LOW();
        camera.set_fifo_burst();
    }

    //releases hold of SPI and camera resources.
    inline void CameraController::EndRead()
    {
        camera.CS_HIGH();
        SPI.endTransaction();
    }


    //Orders camera to begin capturing.
    void CameraController::BeginCapture()
    {
        camera.flush_fifo();
        camera.start_capture();
    }

    //Prepares system for tracking process.
    void CameraController::StartTracking()
    {
        //Resets tracking variables for this tracking process.
        pointsAveraged = 0;
        avgX = avgY = 0;
        currentRow = 0;

        BeginRead();
        SPI.transfer(0x00); // Skip dummy byte
        SkipRows(TOP_MARGIN);
        EndRead();
    }

    //Function runs tracking process over 'trackTimes' rows.
    void CameraController::ProceedTracking(uint16_t trackTimes)
    {
        //If a sufficiently large amount of points have already been identified for the tracking,
        //then no more points will be included.
        //The function is still called to ensure that the major cycle is executed correctly.
        if (pointsAveraged > MAX_AVERAGED_POINTS)
            return;

        BeginRead();
        for (uint16_t row = 0; row < trackTimes; row++)
        {
            SkipColumns(LEFT_MARGIN);
            for (uint16_t col = 0; col < IMAGE_WIDTH; col++)
            {
                uint16_t c565 = SPI.transfer16(0x00);

                //each subexpression of the boolean expression converts c565 into the corresponing colours value
                if ((c565 & 0x1F) < limitR && ((c565 >> 5) & 0x3F) < limitG && ((c565 >> 11) & 0x1F) < limitB)
                {
                    if (++pointsAveraged > MAX_AVERAGED_POINTS)
                        return;

                    avgX += (col - avgX) / pointsAveraged;
                    avgY += (currentRow - avgY) / pointsAveraged;
                }
            }
            SkipColumns(RIGHT_MARGIN);
            SkipRows(ROW_SKIP_COUNT);

            currentRow += (ROW_SKIP_COUNT + 1);
        }
        EndRead();
    }

    //Concludes the tracking by extracting the
    TrackResult CameraController::EndTracking()
    {
        TrackResult result;

        if (pointsAveraged != 0 && pointsAveraged <= MAX_AVERAGED_POINTS)
        {
            result.xCoord = (int16_t)round(avgX) - (int16_t)(IMAGE_WIDTH / 2);
            result.yCoord = (int16_t)round(avgY) - (int16_t)(IMAGE_HEIGHT / 2);
            result.ballFound = true;
        }
        else
        {
            result.xCoord = 0;
            result.yCoord = 0;
            result.ballFound = false;
        }
        
        return result;
    }

    #if OUTPUT_IMAGE
    //Function is used to send image to a processing program capable of displaying it.
    void CameraController::SendImageToProcessing()
    {
        pointsAveraged = 0;
        avgX = avgY = 0;

        //Array to contain pixels of row. A pixel is stored as three bytes, one for each color.
        uint8_t *bytes = (uint8_t *)malloc(IMAGE_WIDTH * 3);

        SerialHelper::AwaitSignal();
        SerialHelper::SendInt(IMAGE_HEIGHT);
        SerialHelper::SendInt(IMAGE_WIDTH);
        
        BeginCapture();
        while (!camera.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) { }

        BeginRead();
        SPI.transfer(0x00); // Skip dummy byte

        SkipRows(TOP_MARGIN);
        for (uint16_t row = 0; row < IMAGE_HEIGHT; row++)
        {
            SkipColumns(LEFT_MARGIN);

            for (uint16_t col = 0; col < IMAGE_WIDTH; col++)
            {
                uint16_t c565 = SPI.transfer16(0x00);

                bytes[col * 3 + 0] = (c565 & 0x1F);         // R
                bytes[col * 3 + 1] = ((c565 >> 5) & 0x3F);  // G
                bytes[col * 3 + 2] = ((c565 >> 11) & 0x1F); // B

                //Since the entire image is sent to processing, then the number of tracked points
                //may exceed the desired limit as time is not a factor.
                if (row % (ROW_SKIP_COUNT + 1) == 0 &&
                    bytes[col * 3 + 0] < limitR && bytes[col * 3 + 1] < limitG && bytes[col * 3 + 2] < limitB)
                {
                    bytes[col * 3 + 0] = 31;
                    bytes[col * 3 + 1] = 0;
                    bytes[col * 3 + 2] = 0;

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

        //The tracked centre is checked to see if it was discovered, and if it was, converted to the
        //coordinate system of the processing system.
        SerialHelper::AwaitSignal();
        SerialHelper::SendInt(pointsAveraged == 0 ? 0 : ((int16_t)round(avgX) - (int16_t)(IMAGE_WIDTH / 2)));
        SerialHelper::AwaitSignal();
        SerialHelper::SendInt(pointsAveraged == 0 ? 0 : ((int16_t)round(avgY) - (int16_t)(IMAGE_HEIGHT / 2)));
    }
    #endif
} // namespace RollingTable