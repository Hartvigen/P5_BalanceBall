#include "AIController.h"

namespace RollingTable
{
    double xEdge, yEdge;
    double xVel, yVel;
    double oldX, oldY;
    bool firstTrack = true;
    double outputWeights[2][6] = {{1.8540885,0.5719164,-2.3648925,-0.9914899,1.2489877,-0.47699964},
                                {-1.2158763,1.7786465,-1.547747,0.8287909,-0.009768188,-0.46802598}};
    double hlWeights[6][6] = {{1.2491459,-1.6336393,2.5097566,-2.648139,-0.11263728,-0.3170579},
                            {1.5949479,0.39890862,1.1823832,2.4798512,-0.040426433,-0.28946704},
                            {-2.9181266,-1.4038169,-1.7521994,-2.4527593,-0.3501675,-0.3442222}, 
                            {-2.9856622,3.150909,-2.0548623,1.9108675,-0.38820553,0.23116457},
                            {-0.4577422,-1.2046074,-1.0166165,0.6889669,-0.44002217,1.2216282},
                            {-0.4522274,2.2131596,1.2934623,1.3025684,-0.28884238,-0.56055343}};
    double hlOut[6];
    double hlBias[6] = {1, 1.0724626, 0.5300939, 1, 1, 1.1963881};
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
   
        for (int i = 0; i < 6; i++)
        {
            hlOut[i] = hlBias[i];
            for (int j = 0; j < 6; j++)
                hlOut[i] += hlWeights[i][j] * input[j];
            
            if(i < 4)
                hlOut[i] = Rectifier(hlOut[i]);
            else
                hlOut[i] = Tipping(Sigmoid(Inverse(hlOut[i])));
        }

        for (int i = 0; i < 2; i++)
        {
            output[i] = outputBias[i];
            for (int j = 0; j < 6; j++)
                output[i] += outputWeights[i][j] * hlOut[j];
            
            output[i] = Tipping(output[i]);
        }

        innerAng = -(output[0] * MAX_ANGLE); // Inner motors are inverted
        outerAng = (output[1] * MAX_ANGLE);
    }

    inline double AIController::Rectifier(double input){return (input < 0 ? 0 : input);}
    inline double AIController::Sigmoid(double input){return 2 / (1 + exp(input))- 1;}
    inline double AIController::Tipping(double input){return 5*pow(input/5, 3);}
    inline double AIController::Inverse(double input){return -15/input;}
} // namespace RollingTable