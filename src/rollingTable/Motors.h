#ifndef MOTORS_H_INCLUDE
#define MOTORS_H_INCLUDE

#include <stdint.h>
#include <Wire.h>
#include <NXShield.h>

namespace RollingTable
{
    class MotorsController
    {
        private:
            NXShield shield;

            int8_t maxAngle = 0;

            int8_t innerAngle = 0;
            int8_t innerDir = 0;
            int8_t innerSpeed = 0;

            int8_t outerAngle = 0;
            int8_t outerDir = 0;
            int8_t outerSpeed = 0;

            void Move(NXShieldBank& bank, int8_t& angle, int8_t& dir, int8_t speed);

        public:
            MotorsController();

            void Reset();
            void SetMaxAngle(int8_t angle);

            void SetInnerAngle(int8_t targetAngle);
            void SetInnerSpeed(int8_t speed);

            void SetOuterAngle(int8_t targetAngle);
            void SetOuterSpeed(int8_t speed);

            void Move();
    };
}

#endif