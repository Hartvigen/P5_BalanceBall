#include "test_CameraController.h"

int16_t xCoor, yCoor;
uint64_t timer;

// All test for CameraController should be placed in here
void TestAll_CameraController()
{
    RUN_TEST(Test_CameraTest);
}

void Test_CameraTest()
{
    bool ballfoundtrue = CameraSetup();
    TEST_ASSERT(ballfoundtrue);
}

bool CameraSetup()
{
    bool ballFound;
    CameraController::BeginCapture();
    for (int i = 9; i--;)
    {
        WaitTimer();
    }

    CameraController::StartTracking();
    for (int i = 13; i--;)
    {
        CameraController::ProceedTracking(3);
        WaitTimer();
    }
    ballFound = CameraController::EndTracking(xCoor, yCoor);
    return ballFound;
}

inline void WaitTimer()
{
    timer = micros();
    while (micros() < timer) { }
    timer += 8000;
}