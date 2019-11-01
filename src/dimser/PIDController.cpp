#include <Arduino.h>
#include <PID_v1.h>
#include <headers/Camera.h>

double setPoint;
double input; //ball posistion
double output; //motor turn

double Kp = 0, Ki = 10, Kd = 0;

PID pid(&input, &output, &setPoint, Kp, Ki, Kd, DIRECT);

void initPID(){
    Serial.begin(9600);
    //hardcode the desired ball posistion
    setPoint = 0;
    //Turn on the PID
    pid.SetMode(AUTOMATIC);
    //Adjust the PID values
    pid.SetTunings(Kp, Ki, Kd);

}

void runPID(){
    pid.Compute();
}