#include <Arduino.h>
#include <unity.h>

int main(){
    UNITY_BEGIN();
    int test1 = 10;
    TEST_ASSERT_EQUAL(test1, 10);

    UNITY_END();
}