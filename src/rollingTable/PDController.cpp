#include "PDController.h"

namespace RollingTable
{
    double proportionalInner = 0, proportionalOuter = 0;
    double derivativeInner = 0, derivativeOuter = 0;
    double xPrev = 0, yPrev = 0;

    void PDController::RunPD(double xCo, double yCo, int8_t &innerAng, int8_t &outerAng)
    {
        proportionalInner = IN_KP * xCo;
        derivativeInner   = IN_KD * (xCo - xPrev) / PERIOD;

        proportionalOuter = OUT_KP * yCo;
        derivativeOuter   = OUT_KD * (yCo - yPrev) / PERIOD;

        xPrev = xCo;
        yPrev = yCo;

        innerAng = (int8_t)(proportionalInner + derivativeInner);
        outerAng = -(int8_t)(proportionalOuter + derivativeOuter); // Motors were inverted unintentionally
    }
}