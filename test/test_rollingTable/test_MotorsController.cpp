#include "test_MotorsController.h"

// All test for MotorsController should be placed in here
void TestAll_MotorsController()
{
    //RUN_TEST(Test_AnglesOfMotors);
}

// Test for seeing if the motor will be set correct
void Test_AnglesOfMotors()
{
    MotorsController::SetOuterAngle(5);
    MotorsController::SetInnerAngle(5);
    MotorsController::Move();
    TEST_ASSERT((int) MotorsController::GetInnerEncoder == 5 &&
                (int) MotorsController::GetOuterEncoder == 5);
}
