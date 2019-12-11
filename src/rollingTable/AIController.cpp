#include "AIController.h"

#if CTRL_AI

namespace RollingTable
{
    double hlOut[4];
    double hlWeights[4][4] = {{1.2491459,-1.6336393,2.5097566,-2.648139},
                            {1.5949479,0.39890862,1.1823832,2.4798512},
                            {-2.9181266,-1.4038169,-1.7521994,-2.4527593}, 
                            {-2.9856622,3.150909,-2.0548623,1.9108675}};

    double output[2];
    double outputWeights[2][4] = {{1.8540885,0.5719164,-2.3648925,-0.9914899},
                                {-1.2158763,1.7786465,-1.547747,0.8287909}};

    double xPrev, yPrev;
    bool firstTrack = true;

    TiltResult AIController::RunNN(TrackResult trackResult)
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
        double input[] { xCo/HALF_WIDTH, yCo/HALF_HEIGHT, xVel/0.1, yVel/0.1 };
   

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
                hlOut[i] += hlWeights[i][j] * input[j];
            
            if(i < 2)
                hlOut[i] = Cntr(hlOut[i]);
            else
                hlOut[i] = Edge(hlOut[i]);
        }

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 4; j++)
                output[i] += outputWeights[i][j] * hlOut[j];
            
            output[i] = Tilt(output[i]);
        }


        result.innerAng = -(output[0] * MAX_ANGLE); // Inner motors are inverted
        result.outerAng = (output[1] * MAX_ANGLE);

        xPrev = xCo;
        yPrev = yCo;
        firstTrack = false;

        return result;
    }

    double AIController::Tilt(double x){return x;}
    double AIController::Edge(double x){return pow(x,3);}
    double AIController::Cntr(double x){return 2/(1 + exp(-5*x)) - 1;}
} // namespace RollingTable

#endif