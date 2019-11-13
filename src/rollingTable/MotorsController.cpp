#include "MotorsController.h"

namespace RollingTable
{
    NXShield MotorsController::shield;

    int8_t MotorsController::innerAngle;
    int8_t MotorsController::innerDir;
    int8_t MotorsController::innerSpeed;

    int8_t MotorsController::outerAngle;
    int8_t MotorsController::outerDir;
    int8_t MotorsController::outerSpeed;

    void MotorsController::Init()
    {
        shield.init(SH_HardwareI2C);
        shield.bank_a.motorStartBothInSync();
        shield.bank_b.motorStartBothInSync();
    }

    void MotorsController::Reset() 
    {
        shield.bank_a.motorReset();
        shield.bank_b.motorReset();
    }


    void MotorsController::SetInnerAngle(int8_t targetAngle) 
    {
        innerAngle = min(targetAngle, MAX_ANGLE);
        innerAngle = max(innerAngle, -MAX_ANGLE);
    }

    void MotorsController::SetInnerSpeed(int8_t speed) 
    {
        innerSpeed = speed;
    }


    void MotorsController::SetOuterAngle(int8_t targetAngle) 
    {
        outerAngle = min(targetAngle, MAX_ANGLE);
        outerAngle = max(outerAngle, -MAX_ANGLE);
    }

    void MotorsController::SetOuterSpeed(int8_t speed) 
    {
        outerSpeed = speed;
    }


    void MotorsController::Move()
    {
        Move(shield.bank_a, innerAngle, innerDir, innerSpeed);
        //Move(shield.bank_b, outerAngle, outerDir, outerSpeed);
    }


    void MotorsController::Move(NXShieldBank& bank, int8_t& angle, int8_t& dir, int8_t speed)
    {
        int8_t encoder = bank.motorGetEncoderPosition(SH_Motor_Both);
        int8_t angleError = angle - encoder;
        int8_t nextDir = angleError > 0 ? 1 : -1;

        angleError = abs(angleError);

        if (dir != 0 && angleError <= 1)
        {
            dir = 0;
            bank.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
        }
        else if (dir != 0 && angleError <= 3)
        {
            dir = 0;
            bank.motorStop(SH_Motor_Both, SH_Next_Action_Float);
        }
        else if (dir != nextDir)
        {
            dir = nextDir;
            bank.motorRunUnlimited(
                SH_Motor_Both, 
                (dir > 0 ? SH_Direction_Forward : SH_Direction_Reverse), 
                speed
            );
        }
    }

    void MotorsController::MoveCircleTest()
    {
        float angle = 0;
        float mod = 3;
        uint64_t calibrateTime = 0;

        MotorsController motors;
        motors.Reset();
        motors.SetInnerSpeed(10);
        motors.SetOuterSpeed(10);
        
        while (true)
        {
            delay(10);
            angle += mod;

            if (millis() > calibrateTime)
            {
                motors.SetInnerAngle(cos(angle*0.0174532925)*10);
                motors.SetOuterAngle(sin(angle*0.0174532925)*10);

                calibrateTime = millis() + 50;
            }

            motors.Move();
        }
    }
}
