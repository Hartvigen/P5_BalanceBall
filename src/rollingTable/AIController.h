#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#define HALF_SIZE 65

#include <stdint.h>
#include <Arduino.h>

namespace RollingTable
{
    class AIController 
    {
        private:
            static double Rectifier(double input);
            static double Sigmoid(double input);
            static double Tipping(double input);
            static double Inverse(double input);
        public:
            //Runs Neural network to get desired angle
            static void RunNN(double xCo, double yCo, int8_t& innerAng, int8_t& outerAng);
    };
}

#endif