#include "AIController.h"

#if CTRL_AI

namespace RollingTable
{
    double xEdge, yEdge;
    double xVel, yVel;
    double oldX, oldY;
    bool firstTrack = true;
    double outputWeights[2][4] = {{1.8540885,0.5719164,-2.3648925,-0.9914899},
                                {-1.2158763,1.7786465,-1.547747,0.8287909}};
    double hlWeights[4][6] = {{1.2491459,-1.6336393,2.5097566,-2.648139,-0.11263728,-0.3170579},
                            {1.5949479,0.39890862,1.1823832,2.4798512,-0.040426433,-0.28946704},
                            {-2.9181266,-1.4038169,-1.7521994,-2.4527593,-0.3501675,-0.3442222}, 
                            {-2.9856622,3.150909,-2.0548623,1.9108675,-0.38820553,0.23116457}};
    double hlOut[4];
    double hlBias[4] = {1, 1.0724626, 0.5300939, 1};
    double outputBias[2] = {1, 1};
    double output[2];
    double input[6];

    void AIController::RunNN(double xCo, double yCo, int8_t &innerAng, int8_t &outerAng)
    {
        xEdge = (xCo < 0 ? HALF_SIZE : -HALF_SIZE) + xCo;
        yEdge = (yCo < 0 ? HALF_SIZE : -HALF_SIZE) + yCo;

        if (firstTrack)
        {
            xVel = (0 - xCo) / PERIOD;
            yVel = (0 - yCo) / PERIOD;
            firstTrack = false;
        }
        else
        {
            xVel = (xCo - oldX) / PERIOD;
            yVel = (yCo - oldY) / PERIOD;
        }

        oldX = xCo;
        oldY = yCo;

        input[0] = xCo;
        input[1] = yCo;
        input[2] = xVel;
        input[3] = yVel;
        input[4] = xEdge;
        input[5] = yEdge;
   
        for (int i = 0; i < 4; i++)
        {
            hlOut[i] = hlBias[i];
            for (int j = 0; j < 6; j++)
                hlOut[i] += hlWeights[i][j] * input[j];
            
            if(i < 2)
                hlOut[i] = Cntr(hlOut[i]);
            else
                hlOut[i] = Edge(hlOut[i]);
        }

        for (int i = 0; i < 2; i++)
        {
            output[i] = outputBias[i];
            for (int j = 0; j < 4; j++)
                output[i] += outputWeights[i][j] * hlOut[j];
            
            output[i] = Tilt(output[i]);
        }

        innerAng = (output[0] * MAX_ANGLE); // Inner motors are inverted
        outerAng = (output[1] * MAX_ANGLE);
    }

   double AIController::Tilt(double x){return 1/1.25*x;}
   double AIController::Edge(double x){return (x < 0 ? (1/exp(abs(x))) : (-1/exp(abs(x))));}
   double AIController::Cntr(double x){return 2/(1 + exp(-x)) - 1;}
} // namespace RollingTable

#endif