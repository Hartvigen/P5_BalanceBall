#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdint.h>

#define PERIOD 5

#define IN_KP 0.2
#define IN_KD 0.2
#define OUT_KP 4
#define OUT_KD 4

namespace RollingTable
{
    class PDController 
    {
        public:
            //Calculates the desired angles using PD controllers
            static void RunPD(double xCo, double yCo, int8_t &innerAng, int8_t &outerAng);
    };
}

#endif