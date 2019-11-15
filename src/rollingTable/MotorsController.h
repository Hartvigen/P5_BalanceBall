#ifndef MOTORS_CONTROLLER_H_INCLUDE
#define MOTORS_CONTROLLER_H_INCLUDE

#include <stdint.h>
#include <NXShield.h>

#define MAX_ANGLE (int8_t)10

namespace RollingTable
{
    class MotorsController
    {
        private:
            static NXShield shield;

            static int8_t innerAngle;
            static int8_t innerDir;
            static int8_t innerSpeed;

            static int8_t outerAngle;
            static int8_t outerDir;
            static int8_t outerSpeed;

            static void Move(NXShieldBank& bank, int8_t& angle, int8_t& dir, int8_t speed);

        public:
            static void Init();
            static void Reset();

            static int32_t GetInnerEncoder();
            static void SetInnerAngle(int8_t targetAngle);
            static void SetInnerSpeed(int8_t speed);

            static int32_t GetOuterEncoder();
            static void SetOuterAngle(int8_t targetAngle);
            static void SetOuterSpeed(int8_t speed);

            static void Move();
            static void Stop();

            static void MoveCircleTest();
    };
}

#endif