#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include "Setup.h"
#include <stdint.h>

#define IN_KP 0.1
#define OUT_KP 0.1

#define IN_KD 2.2
#define OUT_KD 2.2

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