#include "headers/Motor.h"

NXShield nxshield;


void initNxShield(){
    nxshield.init(SH_HardwareI2C);
    nxshield.bank_a.motorReset();
    nxshield.bank_b.motorReset();
}

//"SH_Bank" describes which side of the shield we use
//"SH_Motor" describes which port we use on the chosen bank, counting from the side with cable input to the shield
//"SH_Direction" is either forward or backward
//speed is given as a number between 1-100 with 100 being the fastest
void runMotor(SH_Bank bank, SH_Motor motor, SH_Direction direction, int speed, long degrees)
{

    if (bank == SH_Bank_B){
        nxshield.bank_b.motorRunDegrees(motor, 
                    direction, 
                    speed, 
                    degrees,
                    SH_Completion_Dont_Wait,
                    SH_Next_Action_Brake);

    }
    else
    {
        nxshield.bank_a.motorRunDegrees(motor, 
                    direction, 
                    speed,
                    degrees, 
                    SH_Completion_Dont_Wait,
                    SH_Next_Action_Brake);

    }
}

//Run outer motors, placed in bank a. With direction, speed and degrees.
void runOuterMotors(SH_Direction direction, int speed, long degrees)
{
    nxshield.bank_b.motorRunDegrees(SH_Motor_Both, 
                    direction, 
                    speed,
                    degrees, 
                    SH_Completion_Dont_Wait,
                    SH_Next_Action_Brake);
    Serial.println("Congratulations, the board is stabilized!");
    
}

//Run inner motors, placed in bank b. With direction, speed and degrees.
void runInnerMotors(SH_Direction direction, int speed, long degrees)
{
    nxshield.bank_a.motorRunDegrees(SH_Motor_Both, 
                    direction, 
                    speed,
                    degrees,   
                    SH_Completion_Wait_For,
                    SH_Next_Action_BrakeHold);

}


void printSerialMotorData(float Em){
    Serial.print("Motor 1: ");
    Serial.println(nxshield.bank_b.motorGetEncoderPosition(SH_Motor_Both));
    Serial.print("Em: ");
    Serial.println(Em);
    Serial.println();
}

void stabilize(){
    nxshield.bank_a.motorSetEncoderTarget(SH_Motor_Both, 0);
    bool innerFlag = false;
    bool outerFlag = false;
    int directionFlagInner = 0;
    int directionFlagOuter = 0;


    runInnerMotors(SH_Direction_Forward, 1, 20);
    runOuterMotors(SH_Direction_Forward, 1, 20);
    delay(1000);
    nxshield.bank_a.motorStop(SH_Motor_Both, SH_Next_Action_Float);
    nxshield.bank_b.motorStop(SH_Motor_Both, SH_Next_Action_Float);


    while(!innerFlag || !outerFlag){
        delay(20);
            if(!innerFlag)
                innerFlag = stabilizeInner(directionFlagInner);
            if(!outerFlag)
                outerFlag = stabilizeOuter(directionFlagOuter);
        }
        Serial.println("Congratulations, the board is stabilized!");
}

bool stabilizeInner(int directionFlag)
{
    int curMotor1 = nxshield.bank_a.motorGetEncoderPosition(SH_Motor_1);
    int curMotor2 = nxshield.bank_a.motorGetEncoderPosition(SH_Motor_2);

    if(curMotor1 > 0 && curMotor2 > 0 && directionFlag != 1){
        nxshield.bank_a.motorRunUnlimited(SH_Motor_Both, SH_Direction_Reverse, 1);
        directionFlag = 1;
        }
    
    else if(curMotor1 < 0 && curMotor1 < 0 && directionFlag != 2){
        nxshield.bank_a.motorRunUnlimited(SH_Motor_Both, SH_Direction_Forward, 1);
        directionFlag = 2;
        }

    if(curMotor1 == 0 && curMotor2 == 0) 
    {
        nxshield.bank_a.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
        Serial.println("Inner stable");
        return true;
    }
    Serial.print("Motor 1: ");
    Serial.print(curMotor1);
    Serial.print("     Motor 2: ");
    Serial.println(curMotor2);
    return false;
}

bool stabilizeOuter(int directionFlag)
{
    int curMotor1 = nxshield.bank_b.motorGetEncoderPosition(SH_Motor_1);
    int curMotor2 = nxshield.bank_b.motorGetEncoderPosition(SH_Motor_2);

    if(curMotor1 > 0 && curMotor2 > 0 && directionFlag != 1){
        nxshield.bank_b.motorRunUnlimited(SH_Motor_Both, SH_Direction_Reverse, 1);
        directionFlag = 1;
        }
    
    else if(curMotor1 < 0 && curMotor2 < 0 && directionFlag != 2){
        nxshield.bank_b.motorRunUnlimited(SH_Motor_Both, SH_Direction_Forward, 1);
        directionFlag = 2;
        }

    if(curMotor1 == 0 && curMotor2 == 0) 
    {
        nxshield.bank_b.motorStop(SH_Motor_Both, SH_Next_Action_BrakeHold);
        Serial.println("Outer stable");
        return true;
    }
    Serial.print("Motor 3: ");
    Serial.print(curMotor1);
    Serial.print("     Motor 4: ");
    Serial.println(curMotor2);
    return false;
}

void generalMotorTest(){
    Serial.println(nxshield.bank_b.motorGetEncoderPosition(SH_Motor_1));
    float Em = 0;
    int32_t encode;
    int i = 0;
    delay(2000);

    for(int x = 0; x < 2; x++){
        runOuterMotors(SH_Direction_Reverse, 50, 30);
        delay(1000);

        encode = nxshield.bank_b.motorGetEncoderPosition(SH_Motor_1);
        i += 1;
        Em += (encode-Em)/i;
        printSerialMotorData(Em);

        
        runOuterMotors(SH_Direction_Forward, 50, 60);
        delay(1000);   

        encode = nxshield.bank_b.motorGetEncoderPosition(SH_Motor_1);
        i += 1;
        Em += (encode-Em)/i;
        printSerialMotorData(Em);
        
        
        runOuterMotors(SH_Direction_Reverse, 50, 30);
        delay(1000);                
                encode = nxshield.bank_b.motorGetEncoderPosition(SH_Motor_1);
        i += 1;
        Em += (encode-Em)/i;
        printSerialMotorData(Em);    

        }
        

}