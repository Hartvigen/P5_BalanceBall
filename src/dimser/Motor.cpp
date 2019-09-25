#include "headers/Motor.h"

void runMotor(SH_Bank bank, SH_Motor motor, SH_Direction direction, int speed, long degrees)
{
    NXShield nxshield;
    nxshield.init(SH_HardwareI2C);

    if (bank == SH_Bank_B){
        nxshield.bank_b.motorRunDegrees(motor, 
                    direction, 
                    speed, 
                    degrees,
                    SH_Completion_Dont_Wait,
                    SH_Next_Action_Brake);

    }
    else
    {
        nxshield.bank_a.motorRunDegrees(motor, 
                    direction, 
                    speed,
                    degrees, 
                    SH_Completion_Dont_Wait,
                    SH_Next_Action_Brake);

    }
}

//Run outer motors, placed in bank a. With direction, speed and degrees.
void runOuterMotors(SH_Direction direction, int speed, long degrees)
{
    NXShield nxshield;
    nxshield.init(SH_HardwareI2C);

    nxshield.bank_a.motorRunDegrees(SH_Motor_Both, 
                    direction, 
                    speed,
                    degrees, 
                    SH_Completion_Dont_Wait,
                    SH_Next_Action_Brake);

    
}

//Run inner motors, placed in bank b. With direction, speed and degrees.
void runInnerMotors(SH_Direction direction, int speed, long degrees)
{
    NXShield nxshield;
    nxshield.init(SH_HardwareI2C);

    nxshield.bank_b.motorRunDegrees(SH_Motor_Both, 
                    direction, 
                    speed,
                    degrees, 
                    SH_Completion_Dont_Wait,
                    SH_Next_Action_Brake);

}