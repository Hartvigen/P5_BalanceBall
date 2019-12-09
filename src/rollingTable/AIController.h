#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#define HALF_SIZE 65

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
            static void RunNN(double xCo, double yCo, int8_t& innerAng, int8_t& outerAng);
    };
}

#endif

#endif