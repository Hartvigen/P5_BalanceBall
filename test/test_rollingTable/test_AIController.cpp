#include "test_AIController.h"

int16_t AIxCo = 5, AIyCo = 7;
int8_t AIxAng = 0, AIyAng = 0;


void TestAll_AIController()
{
    AIController::RunNN(AIxCo, AIyCo, AIxAng, AIyAng);
    RUN_TEST(Test_RunNNXAngle);
    RUN_TEST(Test_RunNNYAngle);
}


void Test_RunNNXAngle()
{
    TEST_ASSERT_EQUAL(-99, AIxAng);
}

void Test_RunNNYAngle()
{
    TEST_ASSERT_EQUAL(0, AIyAng);
}