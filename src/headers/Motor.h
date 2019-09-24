#ifndef Motor_h
#define Motor_h

#include <Wire.h>
#include <NXShield.h>

#include <avr/io.h> // header file file for input output pins
#include <util/delay.h> // header file for delay.

extern int runMotor(SH_Bank, SH_Motor, SH_Direction, int, long);
extern void runNX(NXShield);


#endif