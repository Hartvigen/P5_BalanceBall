#include <Arduino.h>
#include <PID_v1.h>
#include <rollingTable/MotorsController.h>

#define MAX_OUTPUT 50

double setPoint; //we only have one setpoint as the desired value for both inner and outer motor is for the ball to be at 0
double innerInput, outerInput; //ball posistion for both inner and outer
double innerOutput, outerOutput; //motor turn
int period = 5;
double integralSum = 0;
float lastInput = 0;
double output = 0;

double IKp = 0.1, OKp = 0.175, IKi = 0, OKi = 0, IKd = 0, OKd = 2;

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

void PIDMotors(int8_t &xAng, int8_t &yAng)
{

}

void runPID(float xCo, float yCo, int8_t &xAng, int8_t &yAng)
{
    double proportional = IKp * xCo;
    double integral     = integralSum + IKi * xCo * period;
    double derivative   = IKd * (xCo - lastInput) / period;

    integralSum = integral;
    lastInput = xCo;

    double result = proportional + integral + derivative;

    output = result;

    xAng = output;

    Serial.print("Posistion: ");
    Serial.print(xCo);


}