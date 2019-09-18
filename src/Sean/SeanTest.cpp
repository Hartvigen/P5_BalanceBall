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