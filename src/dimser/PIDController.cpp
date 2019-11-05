#include <Arduino.h>
#include <PID_v1.h>

double setPoint; //we only have one setpoint as the desired value for both inner and outer motor is for the ball to be at 0
double innerInput, outerInput; //ball posistion for both inner and outer
double innerOutput, outerOutput; //motor turn

double IKp, OKp = 0, IKi, OKi = 0, IKd, OKd = 0;

//We use two seperate PID Controllers as the input for 
PID innerPid(&innerInput, &innerOutput, &setPoint, IKp, IKi, IKd, DIRECT);
PID outerPid(&outerInput, &outerOutput, &setPoint, OKp, OKi, OKd, DIRECT);

void initPID(){
    //hardcode the desired ball posistion
    setPoint = 0;
    //Turn on the PID
    innerPid.SetMode(AUTOMATIC);
    outerPid.SetMode(AUTOMATIC);

}

void runPID(float xCo, float yCo){
    innerInput = xCo;
    outerInput = yCo;

    innerPid.Compute();
    outerPid.Compute();


}