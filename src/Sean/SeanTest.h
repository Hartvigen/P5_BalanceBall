#ifndef SeanTest_H
#define SeanTest_H

#include <Wire.h>
#include <NXShield.h>

#include <SPI.h>
#include <Pixy.h>

#include <avr/io.h> // header file file for input output pins
#include <util/delay.h> // header file for delay.

extern int runSean();
extern void runNX();
extern void runPixy();

#endif