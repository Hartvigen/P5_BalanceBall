#include "test_PIDController.h"

int16_t xCo = 5, yCo = 7;
int8_t xAng = 0, yAng = 0;

// All test for PIDController should be placed in here
void TestAll_PIDController()
{
    PDController::RunPD(xCo, yCo, xAng, yAng);
    RUN_TEST(Test_RunPDXAngle);
    RUN_TEST(Test_RunPDYAngle);

}

void Test_RunPDXAngle()
{
    TEST_ASSERT_EQUAL(2, xAng);
}

void Test_RunPDYAngle()
{
    TEST_ASSERT_EQUAL(-3, yAng);
}