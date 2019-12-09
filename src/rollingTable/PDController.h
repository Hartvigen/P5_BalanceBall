#ifndef PD_CONTROLLER_H
#define PD_CONTROLLER_H

#include "Setup.h"
#include <stdint.h>

#if CTRL_PD

#define IN_KP 0.016
#define OUT_KP 0.016

#define IN_KD 22
#define OUT_KD 22

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

#endif