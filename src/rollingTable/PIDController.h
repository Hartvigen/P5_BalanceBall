#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdint.h>

namespace RollingTable
{
    class PIDController 
    {
        public:
            //Calculates a desired angle using PID controllers
            static void RunPID(double xCo, double yCo, int8_t &innerAng, int8_t &outerAng);
    };
}

#endif