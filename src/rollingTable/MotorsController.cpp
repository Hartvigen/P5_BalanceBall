#include "MotorsController.h"

namespace RollingTable
{
    NXShield MotorsController::shield;

    int8_t MotorsController::innerAngle;
    int8_t MotorsController::innerDir;

    int8_t MotorsController::outerAngle;
    int8_t MotorsController::outerDir;

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


    int32_t MotorsController::GetOuterEncoder()
    {
        return shield.bank_b.motorGetEncoderPosition(SH_Motor_Both);
    }

    void MotorsController::SetOuterAngle(int8_t targetAngle) 
    {
        outerAngle = min(targetAngle, MAX_ANGLE);
        outerAngle = max(outerAngle, -MAX_ANGLE);
    }


    void MotorsController::Move()
    {
        Move(shield.bank_a, innerAngle, innerDir, INNER_SPEED);
        Move(shield.bank_b, outerAngle, outerDir, OUTER_SPEED);
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
        
        if (angleError <= 1)
        {
            if (dir != 0)
            {
                dir = 0;
                bank.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
            }
        }
        else if (dir != nextDir)
        {
            dir = nextDir;
            bank.motorRunUnlimited(
                SH_Motor_Both, 
                (dir > 0 ? SH_Direction_Forward : SH_Direction_Reverse), 
                (angleError <= 4 ? speed/2 : speed)
            );
        }
    }


    void MotorsController::MoveCircleTest()
    {
        float angle = 0;
        float mod = 0.75;
        uint64_t calibrateTime = 0; 
        uint32_t time;

        int8_t xAng = 0, yAng = 0;

        Reset();
        SetOuterAngle(0);
        
        while (true)
        {
            time = millis();
            angle += mod;

            delay(6);
            if (millis() > calibrateTime)
            {
                //calibrateTime =+ 145;

                //SetInnerAngle(cos(angle*0.0174532925)*15);
                //SetOuterAngle(sin(angle*0.0174532925)*15);

                calibrateTime = millis() + 1000;
                
                if (xAng != -15 && xAng != 0)
                {
                    xAng = -15;
                }
                else if (xAng == -15)
                    xAng = 0;
                else
                {
                    xAng = 15;
                }

                SetInnerAngle(xAng);
            }

            Move();

            Serial.print((millis() - time));
            Serial.print("  ");
            Serial.print(GetInnerEncoder());
            Serial.print("  ");
            Serial.println(xAng);
        }
    }
}
