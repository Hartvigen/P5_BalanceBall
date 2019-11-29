#include "PIDController.h"

#define PERIOD 5

namespace RollingTable
{
    double proportionalInner = 0, proportionalOuter = 0;
    double integralInner = 0, integralOuter = 0;
    double derivativeInner = 0, derivativeOuter = 0;
    double lastInputInner = 0, lastInputOuter = 0;

    double IKp = 0.12, IKi = 0.0, IKd = 14.0;
    double OKp = 0.12, OKi = 0.0, OKd = 14.0;


    void PIDController::RunPID(double xCo, double yCo, int8_t &innerAng, int8_t &outerAng)
    {
        //calculate PID values for inner and then outer motors
        proportionalInner = IKp * xCo;
        integralInner     = integralInner + IKi * xCo * PERIOD;
        derivativeInner   = IKd * (xCo - lastInputInner) / PERIOD;

        proportionalOuter = OKp * yCo;
        integralOuter     = integralOuter + OKi * yCo * PERIOD;
        derivativeOuter   = OKd * (yCo - lastInputOuter) / PERIOD;

        lastInputInner = xCo;
        lastInputOuter = yCo;

        innerAng = (int8_t)((proportionalInner + integralInner + derivativeInner) / 2);
        outerAng = -(int8_t)((proportionalOuter + integralOuter + derivativeOuter) / 2);
    }
}