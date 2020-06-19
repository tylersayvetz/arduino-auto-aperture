#include <Servo.h>
Servo servo;

//pins
const int potPin = A0;
const int lightPin = A1;
const int goalIndicator = 5;
const int outOfRangeIndicator = 6;

//vars
int target;
int angle;
int potVal;
int lightVal;
int spd;
int tolerance;
float gain;
int lowAngleLimit;
int highAngleLimit;
int modifier;

void setup() {
  Serial.begin(9600); // open a serial port with the computer. 
  
  pinMode(goalIndicator, OUTPUT);
  pinMode(outOfRangeIndicator, OUTPUT);
  
  servo.attach(9);

//SETTINGS
  lowAngleLimit = 0; //limits of the swing arm. (0 -155)
  highAngleLimit = 90;
  angle = 75; //position 0-155, the starting position of the servo.
  spd = 1;  // 1-3, the base speed of the servo. 
  gain = 6; // expontential gain factor for seeking goal angle. Lesser values are less reactive to extreme inputs, Greater values may cause overcorrection.
  tolerance = 15; // the acceptable error on each side of the goal valuel. eg: + or - 12.
}


void loop() { 
  //read light value
  lightVal = analogRead(lightPin);
  
  //read the target and map to the range of light values
  int rawTarget = analogRead(potPin);
  target = map(rawTarget, 484, 1024, 100, 1024);

  modifier = calculateGain(lightVal, target, gain);

  //set the angle of the servo and value of the indicator light
  if (lightVal < target - tolerance)  {
    digitalWrite(goalIndicator, LOW);

    //decrement the angle if it's in range. -- otherwise throw an indicator light.
    if (angleLowLimit(angle) == false) { 
      angle -= (spd + modifier);
      digitalWrite(outOfRangeIndicator, LOW);
    } else {
      digitalWrite(outOfRangeIndicator, HIGH);
    }
    
  } else if (lightVal > target + tolerance) {
    digitalWrite(goalIndicator, LOW); 

    //increment the angle if it is in range., Otherwise throw out-of-range indicator light
    if (angleHighLimit(angle) == false) {
      angle += (spd + modifier);  
      digitalWrite(outOfRangeIndicator, LOW);
    } else {
      digitalWrite(outOfRangeIndicator, HIGH);
    }

    //else we are at the goal
  } else {
    digitalWrite(goalIndicator, HIGH);
  }

  servo.write(angle);
  
  printValuesForPlotter();
//  printValuesForMonitor();

}

boolean angleLowLimit(int angle) {
  return (angle <= lowAngleLimit) ? true : false;
}

boolean angleHighLimit(int angle) {
   return (angle > highAngleLimit) ? true : false;
}


int calculateGain(int current, int target, float gain) {
  return pow((abs(current - target) * gain / 1000),2);
}

void printValuesForPlotter () {
  Serial.print(angle);
  Serial.print(" ");
  Serial.print(target);
  Serial.print(" ");
  Serial.print(lightVal);
  Serial.print(" ");
  Serial.println(modifier * 100);
}

void printValuesForMonitor () {
  Serial.print("angle: ");
  Serial.print(angle);
  Serial.print("  ||  ");
  Serial.print("target: ");
  Serial.print(target);
  Serial.print("  ||  ");
  Serial.print("lightVal: ");
  Serial.print(lightVal);
  Serial.print("  ||  ");
  Serial.print("modifier: ");
  Serial.println(modifier);
}
