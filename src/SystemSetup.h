#ifndef SETUP_H_INCLUDE
#define SETUP_H_INCLUDE

#include <stdint.h>

// If system should output image and tracking data
#define OUTPUT_IMAGE false

// Where to output status info
#define INTF_TERMINAL false
#define INTF_RVIEWER true

// What policy to use for decision making
#define CTRL_PD false
#define CTRL_AI true

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