#ifndef MOTORS_CONTROLLER_H_INCLUDE
#define MOTORS_CONTROLLER_H_INCLUDE

#include "SystemSetup.h"
#include <stdint.h>
#include <NXShield.h>

#define INNER_SPEED (uint8_t)10
#define OUTER_SPEED (uint8_t)10

namespace RollingTable
{
    class MotorsController
    {
        private:
            static NXShield shield;

            static int8_t innerAngle;
            static int8_t innerDir;

            static int8_t outerAngle;
            static int8_t outerDir;

            //Moves the motor to "angle" at "speed" velocity. "dir" is the current movement direction.
            static void Move(NXShieldBank bank, int8_t angle, int8_t& dir, int8_t speed);

        public:
            //Initializes the motor controller, resets the motor encoders to zero degrees, and stops any movement.
            static void Init();

            //Returns current inner motor encoder value as degrees
            static int32_t GetInnerEncoder();
            //Sets desired angle of inner motors
            static void SetInnerAngle(int8_t desiredAngle);

            //Returns current outer motor encoder value as degrees
            static int32_t GetOuterEncoder();
            //Sets desired angle of outer motors
            static void SetOuterAngle(int8_t desiredAngle);

            //Moves motors towards desired angles.
            static void Move();
    };
}

#endif