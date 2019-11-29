#include "PDController.h"

namespace RollingTable
{
    double IKp = 0.24, IKd = 28.0;
    double OKp = 0.24, OKd = 28.0;
    
    double xPrev = 0, yPrev = 0;

    double proportionalInner = 0, proportionalOuter = 0;
    double derivativeInner = 0, derivativeOuter = 0;


    void PDController::RunPD(double xCo, double yCo, int8_t &innerAng, int8_t &outerAng)
    {
        proportionalInner = IKp * xCo;
        derivativeInner   = IKd * (xCo - xPrev) / PERIOD;

        proportionalOuter = OKp * yCo;
        derivativeOuter   = OKd * (yCo - yPrev) / PERIOD;

        xPrev = xCo;
        yPrev = yCo;

        innerAng = (int8_t)(proportionalInner + derivativeInner);
        outerAng = -(int8_t)(proportionalOuter + derivativeOuter); // Motors were inverted unintentionally
    }
}