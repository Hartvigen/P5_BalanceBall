#include "test_PIDController.h"

int16_t xCo = 5, yCo = 7;
int8_t xAng = 0, yAng = 0;

// Alle PID teste herinde
void TestAll_PIDController()
{
    RUN_TEST(Test_PIDFormel);

}

void Test_PIDFormel()
{
    PIDController::RunPID(xCo, yCo, xAng, yAng);
    TEST_ASSERT(xAng == 7 && yAng == -10);
}