#include "Motors.h"

namespace RollingTable
{
    Motors::Motors()
    {
        shield.init(SH_HardwareI2C);
        shield.bank_a.motorStartBothInSync();
        shield.bank_b.motorStartBothInSync();

        Reset();
    }

    void Motors::Reset() {
        shield.bank_a.motorReset();
        shield.bank_b.motorReset();
    }

    void Motors::SetMaxAngle(int8_t angle){
        maxAngle = angle;
    }


    void Motors::SetInnerAngle(int8_t targetAngle) {
        innerAngle = min(targetAngle, 15);
        innerAngle = max(innerAngle, -15);
    }

    void Motors::SetInnerSpeed(int8_t speed) {
        innerSpeed = speed;
    }


    void Motors::SetOuterAngle(int8_t targetAngle) {
        outerAngle = min(targetAngle, 15);
        outerAngle = max(outerAngle, -15);
    }

    void Motors::SetOuterSpeed(int8_t speed) {
        outerSpeed = speed;
    }


    void Motors::Move()
    {
        Move(shield.bank_a, innerAngle, innerDir, innerSpeed);
        Move(shield.bank_b, outerAngle, outerDir, outerSpeed);
    }


    void Motors::Move(NXShieldBank& bank, int8_t& angle, int8_t& dir, int8_t speed)
    {
        int8_t encoder = bank.motorGetEncoderPosition(SH_Motor_Both);
        int8_t adjust = angle - encoder;
        int8_t nextDir = adjust > 0 ? 1 : -1;

        adjust = abs(adjust);

        if (dir != 0 && adjust <= 1)
        {
            dir = 0;
            bank.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
        }
        else if (dir != 0 && adjust <= 3)
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
}
