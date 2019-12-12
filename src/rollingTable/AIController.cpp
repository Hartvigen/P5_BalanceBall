#include "AIController.h"

#if CTRL_AI

namespace RollingTable
{
    double hlWeights[4][4] = {  
        {-0.26396087, 0.17738606,-1.04795900, 0.30522442},
        {-0.09836733, 0.16733426, 0.85635210, 1.41451500},
        {-0.06680263, 0.01970565,-0.15914561,-0.05600453},
        { 0.37515295,-0.08730203,-0.49338546,-0.56916930},
    };
    double outputWeights[2][4] = {
        {-0.37129474, 0.36047890, 0.20418686, 0.61584735},
        { 0.03696144, 0.51748920,-0.10813793, 0.16429268},
    };

    double xPrev, yPrev;
    bool firstTrack = true;

    TiltResult AIController::RunNN(TrackResult trackResult)
    {
        TiltResult result = { 0, 0 };
        if (!trackResult.ballFound) {
            firstTrack = true;
            return result;
        }

        double xCo = trackResult.xCoord;
        double yCo = trackResult.yCoord;
        double xVel = (firstTrack ? (0 - xCo) : (xCo - xPrev)) / PERIOD;
        double yVel = (firstTrack ? (0 - yCo) : (yCo - yPrev)) / PERIOD;
        double input[] { xCo/HALF_WIDTH, yCo/HALF_HEIGHT, xVel/0.1, yVel/0.1 };

        double hlOut[4] = { 0,0,0,0 };
        for (int i = 0; i < 4; i++) 
        {
            for (int j = 0; j < 4; j++)
                hlOut[i] += hlWeights[i][j] * input[j];
            
            if (i<2) hlOut[i] = C(hlOut[i]);
            else     hlOut[i] = E(hlOut[i]);
        }

        double output[2] = { 0,0 };
        for (int i = 0; i < 2; i++) 
        {
            for (int j = 0; j < 4; j++)
                output[i] += outputWeights[i][j] * hlOut[j];
            
            output[i] = T(output[i]);
        }

        result.innerAng = -(output[0] * MAX_ANGLE); // Inner motors are inverted
        result.outerAng = (output[1] * MAX_ANGLE);

        xPrev = xCo;
        yPrev = yCo;
        firstTrack = false;

        return result;
    }

    inline double AIController::T(double x) {return x;}
    inline double AIController::E(double x) {return pow(x,3);}
    inline double AIController::C(double x) {return 2/(1 + exp(-5*x)) - 1;}
} // namespace RollingTable

#endif