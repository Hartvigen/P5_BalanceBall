#include "PDController.h"

#if CTRL_PD

namespace RollingTable
{
    double xPrev = 0, yPrev = 0;

    TiltResult PDController::RunPD(double xCo, double yCo)
    {
        TiltResult result;

        double proportionalInner = IN_KP * xCo;
        double derivativeInner   = IN_KD * (xCo - xPrev) / PERIOD;

        double proportionalOuter = OUT_KP * yCo;
        double derivativeOuter   = OUT_KD * (yCo - yPrev) / PERIOD;

        result.innerAng = (int8_t)(proportionalInner + derivativeInner);
        result.outerAng = -(int8_t)(proportionalOuter + derivativeOuter); // Motors are inverted

        xPrev = xCo;
        yPrev = yCo;

        return result;
    }
}

#endif