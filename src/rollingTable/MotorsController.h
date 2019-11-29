#ifndef MOTORS_CONTROLLER_H_INCLUDE
#define MOTORS_CONTROLLER_H_INCLUDE

#include <stdint.h>
#include <NXShield.h>

#define MAX_ANGLE (int8_t)10
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

            //Moves the motor "angle" degrees in "dir" direction at "speed" velocity.
            static void Move(NXShieldBank& bank, int8_t& angle, int8_t& dir, int8_t speed);

        public:
            //Initializes the motor controller
            static void Init();
            //Resets the motor encoders, and stops any movement.
            static void Reset();

            //Returns current inner motor encoder value as degrees
            static int32_t GetInnerEncoder();
            //Sets desired angle of inner motors
            static void SetInnerAngle(int8_t targetAngle);

            //Returns current outer motor encoder value as degrees
            static int32_t GetOuterEncoder();
            //Sets desired angle of outer motors
            static void SetOuterAngle(int8_t targetAngle);

            //Moves motors towards desired angle.
            static void Move();

            //Test function which moves the board in circles.
            static void MoveCircleTest();
    };
}

#endif