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

        shield.bank_a.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
        shield.bank_b.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
    }


    int32_t MotorsController::GetInnerEncoder()
    {
        return shield.bank_a.motorGetEncoderPosition(SH_Motor_Both);
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


    int32_t MotorsController::GetOuterEncoder()
    {
        return shield.bank_b.motorGetEncoderPosition(SH_Motor_Both);
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
        Move(shield.bank_b, outerAngle, outerDir, outerSpeed);
    }

    void MotorsController::Stop()
    {
        shield.bank_a.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
        shield.bank_b.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
    }


    void MotorsController::Move(NXShieldBank& bank, int8_t& angle, int8_t& dir, int8_t speed)
    {
        int8_t encoder = bank.motorGetEncoderPosition(SH_Motor_Both);
        int8_t angleError = angle - encoder;
        int8_t nextDir = (angleError > 0 ? 1 : (angleError < 0 ? -1 : 0));

        angleError = abs(angleError);
        
        //Serial.print(angleError); Serial.print(" "); Serial.print(innerDir); Serial.print(" "); Serial.println(outerDir);
        
        if (angleError <= 1)
        {
            if (dir != 0)
            {
                dir = 0;
                bank.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
            }
        }
        /*
        else if (dir != 0 && angleError <= 2)
        {
            dir = 0;
            bank.motorStop(SH_Motor_Both, SH_Next_Action_Float);
        }
        //*/
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
        float mod = 0.75;
        uint64_t calibrateTime = 0;

        Reset();
        SetInnerSpeed(20);
        SetOuterSpeed(20);
        
        while (true)
        {
            angle += mod;

            if (millis() > calibrateTime)
            {
                calibrateTime =+ 145;

                SetInnerAngle(cos(angle*0.0174532925)*15);
                SetOuterAngle(sin(angle*0.0174532925)*15);

                Move();
            }
        }
    }
}
