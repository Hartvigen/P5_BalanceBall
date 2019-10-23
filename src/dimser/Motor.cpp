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

void generalMotorTest(){
    Serial.println(nxshield.bank_a.motorGetEncoderPosition(SH_Motor_1));
    float Em = 0;
    int32_t encode;
    int i = 0;
    delay(2000);

    for(int x = 0; x < 5; x++){
        runInnerMotors(SH_Direction_Reverse, 50, 34);
        delay(1000);
        encode = nxshield.bank_a.motorGetEncoderPosition(SH_Motor_1);
        Serial.print("Motor 1: ");
        Serial.println(encode);
        i += 1;
        Em += (encode-Em)/i;
        Serial.print("Em: ");
        Serial.println(Em);
        
        runInnerMotors(SH_Direction_Forward, 50, 60);
        delay(1000);                   
                encode = nxshield.bank_a.motorGetEncoderPosition(SH_Motor_1);
        Serial.print("Motor 1: ");
        Serial.println(encode);
        i += 1;
        Em += (encode-Em)/i;
        Serial.print("Em: ");
        Serial.println(Em);

        runInnerMotors(SH_Direction_Reverse, 50, 30);
        delay(1000);                
                encode = nxshield.bank_a.motorGetEncoderPosition(SH_Motor_1);
        Serial.print("Motor 1: ");
        Serial.println(encode);
        i += 1;
        Em += (encode-Em)/i;
        Serial.print("Em: ");
        Serial.println(Em);
        Serial.print("Voltage: ");
        Serial.println(nxshield.bank_a.nxshieldGetBatteryVoltage());
        
        

        }
        

}