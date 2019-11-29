#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdint.h>

#define PERIOD 5

namespace RollingTable
{
    class PDController 
    {
        public:
            //Calculates a desired angle using PID controllers
            static void RunPD(double xCo, double yCo, int8_t &innerAng, int8_t &outerAng);
    };
}

#endif