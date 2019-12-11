#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#define HALF_WIDTH 65.5
#define HALF_HEIGHT 59.5

#include "Setup.h"
#include <stdint.h>
#include <Arduino.h>

#if CTRL_AI

namespace RollingTable
{
    class AIController 
    {
        private:
            static double Cntr(double x);
            static double Edge(double x);
            static double Tilt(double x);
        public:
            //Runs Neural network to get desired angle
            static TiltResult RunNN(TrackResult trackResult);
    };
}

#endif

#endif