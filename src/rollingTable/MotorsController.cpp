#include "MotorsController.h"

namespace RollingTable
{
    NXShield MotorsController::shield;

    int8_t MotorsController::innerAngle = 0;
    int8_t MotorsController::innerDir = 0;

    int8_t MotorsController::outerAngle = 0;
    int8_t MotorsController::outerDir = 0;


    void MotorsController::Init()
    {
        shield.init(SH_HardwareI2C);
        shield.bank_a.motorStartBothInSync();
        shield.bank_b.motorStartBothInSync();

        shield.bank_a.motorReset();
        shield.bank_b.motorReset();

        shield.bank_a.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
        shield.bank_b.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
    }


    int32_t MotorsController::GetInnerEncoder()
    {
        return shield.bank_a.motorGetEncoderPosition(SH_Motor_Both);
    }

    void MotorsController::SetInnerAngle(int8_t desiredAngle) 
    {
        innerAngle = min(desiredAngle, MAX_ANGLE);
        innerAngle = max(innerAngle, -MAX_ANGLE);
    }


    int32_t MotorsController::GetOuterEncoder()
    {
        return shield.bank_b.motorGetEncoderPosition(SH_Motor_Both);
    }

    void MotorsController::SetOuterAngle(int8_t desiredAngle) 
    {
        outerAngle = min(desiredAngle, MAX_ANGLE);
        outerAngle = max(outerAngle, -MAX_ANGLE);
    }


    void MotorsController::Move()
    {
        Move(shield.bank_a, innerAngle, innerDir, INNER_SPEED);
        Move(shield.bank_b, outerAngle, outerDir, OUTER_SPEED);
    }

    void MotorsController::Move(NXShieldBank bank, int8_t desiredAngle, int8_t& dir, int8_t speed)
    {
        int8_t currentAngle = bank.motorGetEncoderPosition(SH_Motor_Both);
        int8_t angleError = desiredAngle - currentAngle;
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
}
