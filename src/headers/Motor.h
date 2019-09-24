#ifndef motor_h
#define motor_h

#include <Wire.h>
#include <NXShield.h>

#include <NXTCam.h>

#include <avr/io.h> // header file file for input output pins
#include <util/delay.h> // header file for delay.

extern void runNX();
extern int runMotor(SH_Bank, SH_Motor, SH_Direction, int, long);



#endif