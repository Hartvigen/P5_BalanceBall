#include "test_CameraController.h"


// Alle camera teste herind
void TestAll_CameraController(){
    RUN_TEST(Test_SizeOfCameraFIFO);

}


// Test størrelsen på billede er det rigtige(When done!)
void Test_SizeOfCameraFIFO(){
    ArduCAM camera;
    uint32_t size = camera.read_fifo_length();
    TEST_ASSERT_EQUAL(size, 8388607);
    
}