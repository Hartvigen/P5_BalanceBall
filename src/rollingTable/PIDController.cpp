#include "PIDController.h"

#define MAX_OUTPUT 50
#define PERIOD 5

namespace RollingTable
{
    double integralSumInner = 0, integralSumOuter = 0;
    double lastInputInner = 0, lastInputOuter = 0;
    double outputInner = 0, outputOuter = 0;

    double IKp = 0.12, IKi = 0.0, IKd = 14.0;
    double OKp = 0.12, OKi = 0.0, OKd = 14.0;


    void PIDController::RunPID(double xCo, double yCo, int8_t &xAng, int8_t &yAng)
    {
        //calculate PID values for inner then outer motors
        double proportionalInner = IKp * xCo;
        double integralInner     = integralSumInner + IKi * xCo * PERIOD;
        double derivativeInner   = IKd * (xCo - lastInputInner) / PERIOD;

        double proportionalOuter = OKp * yCo;
        double integralOuter     = integralSumOuter + OKi * yCo * PERIOD;
        double derivativeOuter   = OKd * (yCo - lastInputOuter) / PERIOD;

        integralSumInner = integralInner;
        integralSumOuter = integralOuter;

        lastInputInner = xCo;
        lastInputOuter = yCo;

        xAng = (int8_t)((proportionalInner + integralInner + derivativeInner) / 2);
        yAng = -(int8_t)((proportionalOuter + integralOuter + derivativeOuter) / 2);
    }
}