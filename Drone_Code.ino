#include <Grove_I2C_Motor_Driver.h>

//Initial value DO NOT TOUCH
String value = "F";
//Intialization of inputs.
int inputMap[] = {511, 511, 1, 511, 511, 1};
//Mapped values of the input array to variables.
int leftY, leftX, rightY, rightX;
boolean isStrafingLeft, isStrafingRight;
#define RearDriver 0x0e     //Actually works (maybe)
#define FrontDriver 0x03    //does?
//com 5

void setup() {
  Serial.begin(9600);
  Motor.begin(0, RearDriver);
  Motor.begin(1, FrontDriver);
}

void loop() {
  if (value == " ") {
    if (Serial.available() > 0) {
      value = Serial.readStringUntil(';');
      deserialize(value, ':');
      mapValues();
    }
  } else {
    Serial.print('R');
    value = " ";
  }
  printState();
  Motor.speed(0, MOTOR2, leftY);
  Motor.speed(1, MOTOR1, rightY);
}

void printState() {
  Serial.println("[" + String(leftY) + ":" + String(leftX) + ":" + String(isStrafingLeft) + ":" + String(rightY) + ":" + String(rightX) + ":" + String(isStrafingRight) + "]");
}

int getBounds(int value) {
  if ((-7 < value) && (value < 5)) {
    return 0;
  }
  return value;
}

int scale(int value){
  return map(value, 0, 1023, -100, 100);
}

void mapValues() {
  leftY = getBounds(scale(inputMap[0])), leftX = getBounds(scale(inputMap[1]));
  rightY = getBounds(scale(inputMap[3])), rightX = getBounds(scale(inputMap[4]));
  isStrafingLeft = (inputMap[2] == 1) ? false : true;
  isStrafingRight = (inputMap[5] == 1) ? false : true;
}

void deserialize(String arg, char seperator) {
  int index = 0;
  String built = "";
  for (char c : arg) {
    if (c != seperator) {
      built = built + c;
    } else {
      inputMap[index] = built.toInt();
      built = "";
      index++;
    }
  }
}
