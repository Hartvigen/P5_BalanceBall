#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "SystemSetup.h"
#include <stdint.h>
#include <Arduino.h>

#if CTRL_AI

#define HALF_WIDTH 65.5
#define HALF_HEIGHT 59.5

namespace RollingTable
{
    class AIController 
    {
        private:
            static double C(double x);
            static double E(double x);
            static double T(double x);
        public:
            //Runs Neural network to get desired angle
            static TiltResult RunNN(TrackResult trackResult);
    };
}

#endif

#endif