#include "test_CameraController.h"

int16_t xCoor = 5, yCoor = 7;
// Alle camera teste herind
void TestAll_CameraController()
{
    RUN_TEST(Test_SizeOfCameraFIFO);
    RUN_TEST(Test_CameraTest);
}


// Test størrelsen på billede er det rigtige(When done!)
void Test_SizeOfCameraFIFO()
{
    ArduCAM camera;
    uint32_t size = camera.read_fifo_length();
    TEST_ASSERT_EQUAL(size, 8388607);
    
}

void Test_CameraTest()
{
    CameraController::BeginCapture();
    CameraController::StartTracking();
    CameraController::ProceedTracking(39);
    bool booll = CameraController::EndTracking(xCoor,yCoor);
    TEST_IGNORE_MESSAGE(BoolToString(booll));
}

inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}