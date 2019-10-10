#include "SeanTest.h"

int runSean(void)
{
    // Set all B ports as output
    DDRB = 0xFF;

    while(1)
    {
        /* set pin 7 high to turn led on */
        PORTB |= _BV(PORTB7);
        _delay_ms(500);

        /* set pin 7 low to turn led off */
        PORTB &= ~_BV(PORTB7);
        _delay_ms(500);
    }

    return 1; 
}

void runNX()
{
    NXShield nxshield;
    nxshield.init(SH_HardwareI2C);

    nxshield.bank_a.motorReset();
    nxshield.bank_a.motorRunUnlimited(SH_Motor_1, 
                     SH_Direction_Forward, 
                     50);
}