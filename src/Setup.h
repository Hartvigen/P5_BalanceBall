#ifndef SETUP_H_INCLUDE
#define SETUP_H_INCLUDE

#include <stdint.h>

// Testing of image output
#define USE_IMG_DIS false

// How should the table be controlled
#define CTRL_PD true
#define CTRL_AI false
#define CTRL_MANUAL false

// How to send commands and recieve output
#define INTF_TERMINAL false
#define INTF_RVIEWER true

// Here because used multiple places
#define MAX_ANGLE (int8_t)5
#define PERIOD 184

struct {
    int8_t innerAng;
    int8_t outerAng;
} typedef TiltResult;

struct {
    int8_t xCoord;
    int8_t yCoord;
    bool ballFound;
} typedef TrackResult;

#endif