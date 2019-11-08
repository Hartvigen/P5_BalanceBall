#include <Arduino.h>
#include <PID_v1.h>

double setPoint; //we only have one setpoint as the desired value for both inner and outer motor is for the ball to be at 0
double innerInput, outerInput; //ball posistion for both inner and outer
double innerOutput, outerOutput; //motor turn

double IKp = 1, OKp = 1, IKi = 0, OKi = 0, IKd = 0, OKd = 0;

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
    innerInput = abs(xCo) * -1;
    outerInput = abs(yCo) * -1;

    innerPid.Compute();
    outerPid.Compute();

    if(xCo > setPoint)
        Serial.println("forward");

    else if(xCo <= setPoint)
        Serial.println("backwards");


    if(yCo > setPoint)
        Serial.println("forward");

    else if(yCo <= setPoint)
        Serial.println("backwards");

    Serial.print("inner: ");
    Serial.print(innerOutput);
    Serial.print("  ");
    Serial.print("outer: ");
    Serial.println(outerOutput);


}