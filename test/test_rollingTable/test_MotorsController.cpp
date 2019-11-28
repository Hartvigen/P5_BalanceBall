#include "test_MotorsController.h"

// Alle motor teste herinde
void TestAll_MotorsController(){
//    RUN_TEST(Test_AnglesOfInnerMotors);
//    RUN_TEST(Test_AnglesOfOuterMotors);
}

// Test om fejlmargin i forhold til moterne(When done)

void Test_AnglesOfOuterMotors(){
    MotorsController::SetOuterAngle(5);
    TEST_ASSERT_EQUAL(5, MotorsController::GetOuterEncoder());
}

void Test_AnglesOfInnerMotors(){
    MotorsController::SetInnerAngle(5);
    TEST_ASSERT_EQUAL(5, MotorsController::GetInnerEncoder());

}
