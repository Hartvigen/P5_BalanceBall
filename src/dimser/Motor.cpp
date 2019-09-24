#include "headers/Motor.h"

int runMotor(SH_Bank bank, SH_Motor motor, SH_Direction direction, int speed, long degrees)
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
 
    return 1;   
}