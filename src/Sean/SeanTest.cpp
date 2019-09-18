#include "SeanTest.h"

int runSean(void)
{
    /* set pin 5 of PORTB for output*/
    DDRB = 0xFF;

    while(1)
    {
        /* set pin 5 high to turn led on */
        PORTB |= _BV(PORTB7);
        _delay_ms(500); //delay 1 second

        /* set pin 5 low to turn led off */
        PORTB &= ~_BV(PORTB7);
        _delay_ms(500); //delay 1 second.
    }

    return 1;   
}

void runNX()
{
    NXShield nxshield;
    nxshield.init(SH_HardwareI2C);
    nxshield.bank_a.motorReset();
    
    nxshield.bank_a.motorRunDegrees(SH_Motor_1, 
                     SH_Direction_Forward, 
                     50,
                     1000, 
                     SH_Completion_Dont_Wait,
                     SH_Next_Action_BrakeHold);
}