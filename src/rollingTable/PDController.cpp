#include "PDController.h"

#if CTRL_PD

namespace RollingTable
{
    double xPrev, yPrev;
    bool firstTrack = true;

    TiltResult PDController::RunPD(TrackResult trackResult)
    {
        TiltResult result = { 0, 0 };
        if (!trackResult.ballFound)
        {
            firstTrack = true;
            return result;
        }

        double xCo = trackResult.xCoord;
        double yCo = trackResult.yCoord;
        double xVel = (firstTrack ? (0 - xCo) : (xCo - xPrev)) / PERIOD;
        double yVel = (firstTrack ? (0 - yCo) : (yCo - yPrev)) / PERIOD;

        double proportionalInner = IN_KP * xCo;
        double derivativeInner   = IN_KD * xVel;

        double proportionalOuter = OUT_KP * yCo;
        double derivativeOuter   = OUT_KD * yVel

        result.innerAng = (int8_t)(proportionalInner + derivativeInner);
        result.outerAng = -(int8_t)(proportionalOuter + derivativeOuter); // Motors are inverted

        xPrev = trackResult.xCoord;
        yPrev = trackResult.yCoord;

        return result;
    }
}

#endif