#ifndef PD_CONTROLLER_H
#define PD_CONTROLLER_H

#include "Setup.h"
#include <stdint.h>

#if CTRL_PD

#define IN_KP 0.025
#define OUT_KP 0.025

#define IN_KD 30
#define OUT_KD 30

namespace RollingTable
{
    class PDController
    {
        public:
            //Calculates the desired angles using PD controllers
            static TiltResult RunPD(double xCo, double yCo);
    };
}

#endif

#endif