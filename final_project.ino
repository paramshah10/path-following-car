#include <ECE3.h>

uint16_t sensorValues[8];
float weights[8] = {-0.7,-0.4,-0.2,-0.1,0.1,0.2,0.4,0.7};

const float kp = 0.094;
const float targetValue = -11.8;

const int left_nslp_pin=31; // nslp ==> awake & ready for PWM
const int left_dir_pin=29;
const int left_pwm_pin=40;

const int right_nslp_pin=11; // nslp ==> awake & ready for PWM
const int right_dir_pin=20;
const int right_pwm_pin=39;

const int leftSpd = 90;
const int rightSpd = 90;

int stp = 0;
///// LED
//const int yellowleft = 51;
//const int yellowright = 41;
//const int redright = 58;
//const int redleft = 57;

void setup()
{
  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);

  digitalWrite(left_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);

  pinMode(right_nslp_pin,OUTPUT);
  pinMode(right_dir_pin,OUTPUT);
  pinMode(right_pwm_pin,OUTPUT);

  digitalWrite(right_dir_pin,LOW);
  digitalWrite(right_nslp_pin,HIGH);

//  /// LED Setup
//  pinMode(yellowleft, OUTPUT);
//  pinMode(yellowright, OUTPUT);
//  pinMode(redleft, OUTPUT);
//  pinMode(redright, OUTPUT);
  
  ECE3_Init();
  Serial.begin(9600); // set the data rate in bits per second for serial data transmission
  delay(2000);
}

float rightSpdxx = leftSpd;
float leftSpdxx = leftSpd;

float firstlastloop = 0;
float secondlastloop = 0;
float thirdlastloop = 0;

float fusedValue = 0;
int counter = 0;

void loop()
{
  fusedValue = 0;
  
  analogWrite(left_pwm_pin,leftSpdxx);
  analogWrite(right_pwm_pin,rightSpdxx);
  // read raw sensor values
  ECE3_read_IR(sensorValues);

  // print the sensor values as numbers from 0 to 2500, where 0 means maximum reflectance and
  // 2500 means minimum reflectance
  fusedValue += sensorValues[0]*(weights[0]);
  fusedValue += sensorValues[1]*(weights[1]);
  fusedValue += sensorValues[2]*(weights[2]);
  fusedValue += sensorValues[3]*(weights[3]);
  fusedValue += sensorValues[4]*(weights[4]);
  fusedValue += sensorValues[5]*(weights[5]);
  fusedValue += sensorValues[6]*(weights[6]);
  fusedValue += sensorValues[7]*(weights[7]);

  if ((sensorValues[0] > 2000 && sensorValues[1] > 2000 && sensorValues[2] > 2000 && sensorValues[3] > 2000) || (sensorValues[4] > 2000 && sensorValues[5] > 2000 && sensorValues[6] > 2000 && sensorValues[7] > 2000))
  {
    counter++;
  }
  else
  {
    counter = 0;
  }  
  if (counter >= 2)
  {
    stp++;

    if (stp == 2)
    {
       analogWrite(left_pwm_pin,0);
       analogWrite(right_pwm_pin,0);      
       exit(0);
    }
    analogWrite(left_pwm_pin,0);
    analogWrite(right_pwm_pin,0);
    delay(100);
    int turn = 95;      
    digitalWrite(left_dir_pin,HIGH);     
    analogWrite(left_pwm_pin,turn);
    analogWrite(right_pwm_pin,turn);
  
    delay(700);
    digitalWrite(left_dir_pin,LOW);

    analogWrite(left_pwm_pin,leftSpd);
    analogWrite(right_pwm_pin,rightSpd);
    
    loop();
  }

  thirdlastloop = secondlastloop;
  secondlastloop = firstlastloop;
  firstlastloop = fusedValue;

  float average = (firstlastloop + secondlastloop + thirdlastloop)/3;
  
  float error = targetValue - average;
//  Serial.println(error);
  float t = kp*error;  //if t < 0 then go left, if t > 0 then go right  

  if (t < 0)
  {
    leftSpdxx = leftSpd+t;
  }
  else
  {
    rightSpdxx = rightSpd-t;
  }
}
