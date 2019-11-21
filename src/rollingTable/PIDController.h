#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdint.h>

namespace RollingTable
{
    class PIDController 
    {
        public:
            static void RunPID(double xCo, double yCo, int8_t &xAng, int8_t &yAng);
    };
}

#endif