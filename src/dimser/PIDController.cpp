#include <Arduino.h>
#include <PID_v1.h>
#include <rollingTable/MotorsController.h>

#define MAX_OUTPUT 50

double setPoint; //we only have one setpoint as the desired value for both inner and outer motor is for the ball to be at 0
double innerInput, outerInput; //ball posistion for both inner and outer
double innerOutput, outerOutput; //motor turn
int period = 5;
double integralSumInner = 0, integralSumOuter = 0;
float lastInputInner = 0, lastInputOuter = 0;
double outputInner = 0, outputOuter = 0;

double IKp = 0.25, OKp = 0.2, IKi = 0, OKi = 0, IKd = 0, OKd = 0;

//We use two seperate PID Controllers as the input for 
PID innerPid(&innerInput, &innerOutput, &setPoint, IKp, IKi, IKd, DIRECT);
PID outerPid(&outerInput, &outerOutput, &setPoint, OKp, OKi, OKd, DIRECT);

void initPID()
{
    //hardcode the desired ball posistion
    setPoint = 0;
    //Turn on the PID
    innerPid.SetMode(AUTOMATIC);
    outerPid.SetMode(AUTOMATIC);
}

void runPID(float xCo, float yCo, int8_t &xAng, int8_t &yAng)
{
    //calculate PID values for inner then outer motors
    double proportionalInner = IKp * xCo;
    double integralInner     = integralSumInner + IKi * xCo * period;
    double derivativeInner   = IKd * (xCo - lastInputInner) / period;

    double proportionalOuter = OKp * yCo;
    double integralOuter     = integralSumOuter + OKi * yCo * period;
    double derivativeOuter   = OKd * (yCo - lastInputOuter) / period;

    integralSumInner = integralInner;
    integralSumOuter = integralOuter;

    lastInputInner = xCo;
    lastInputOuter = yCo;

    outputInner = proportionalInner + integralInner + derivativeInner;
    outputOuter = proportionalOuter + integralOuter + derivativeOuter;

    xAng = outputInner > 10 ? 10 : outputInner;
    yAng = outputOuter > 10 ? 10 : outputOuter;

}