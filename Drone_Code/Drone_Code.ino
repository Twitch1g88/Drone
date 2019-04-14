#include <Grove_I2C_Motor_Driver.h>

//Initial value DO NOT TOUCH
String value = "F";
//Intialization of inputs.
int inputMap[] = {511, 511, 1, 511, 511, 1};
//Mapped values of the input array to variables.
int leftY, leftX, rightY, rightX;
boolean isLeftJoypadDown, isRightJoypadDown;
int currentMovement[] = {0, 0, 0, 0};
//Driver Addresses
#define RearDriver 0x0e
#define FrontDriver 0x03

//Defining WheelState's because for some reason I can't have valued enums.
//STATE {fl, fr, bl, br}
int LONG_LEFT_FORWARD[] = {50, 100, 50, 100};
int FORWARD[] = {100, 100, 100, 100};
int LONG_RIGHT_FORWARD[] = {100, 50, 100, 50};
int POINT_LEFT[] = { -100, 100, -100, 100};
int STATIONARY[] = {0, 0, 0, 0};
int POINT_RIGHT[] = {100, -100, 100, -100};
int LONG_LEFT_BACKWARD[] = { -50, -100, -50, -100};
int BACKWARD[] = { -100, -100, -100, -100};
int LONG_RIGHT_BACKWARD[] = { -100, -50, -100, -50};
int STRAFE_UP_LEFT[] = {100, 0, 0, -100};
int STRAFE_UP_RIGHT[] = {0, 100, -100, 0};
int STRAFE_LEFT[] = { -100, 100, 100, -100};
int STRAFE_RIGHT[] = {100, -100, -100, 100};
int STRAFE_DOWN_LEFT[] = { -100, 0, 0, 100};
int STRAFE_DOWN_RIGHT[] = {0, -100, 100, 0};

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
      assignValues();
      moveDrone();
    }
  } else {
    Serial.print('R');
    value = " ";
  }
}

void moveDrone() {
  if ((leftX == 0) && (leftY == 0)){
    stopMotors();
    return;
  }

  if(leftX == 0){
    if(leftY > 0){
      assignMovement(FORWARD);
    }else{
      assignMovement(BACKWARD);
    }
    moveMotors(currentMovement, getScale(leftY, leftX));
    return;
  }
    
  if (!isRightJoypadDown) {
    if (leftY == 0) {
      if (leftX > 0) {
        assignMovement(POINT_RIGHT);
      } else {
        assignMovement(POINT_LEFT);
      }
    } else {
      if ((leftX > 0) && (leftY > 0)) {
        assignMovement(LONG_RIGHT_FORWARD);
      } else if ((leftX < 0) && (leftY < 0)) {
        assignMovement(LONG_LEFT_BACKWARD);
      } else if ((leftX > 0) && (leftY < 0)) {
        assignMovement(LONG_RIGHT_BACKWARD);
      } else if ((leftX < 0) && (leftY > 0)) {
        assignMovement(LONG_LEFT_FORWARD);
      }
    }
  } else {
    if (leftY == 0) {
      if (leftX > 0) {
        assignMovement(STRAFE_RIGHT);
      } else {
        assignMovement(STRAFE_LEFT);
      }
    } else {
      if ((leftX > 0) && (leftY > 0)) {
        assignMovement(STRAFE_UP_RIGHT);
      } else if ((leftX < 0) && (leftY < 0)) {
        assignMovement(STRAFE_DOWN_LEFT);
      } else if ((leftX > 0) && (leftY < 0)) {
        assignMovement(STRAFE_DOWN_RIGHT);
      } else if ((leftX < 0) && (leftY > 0)) {
        assignMovement(STRAFE_UP_LEFT);
      }
    }
  }
  moveMotors(currentMovement, getScale(leftY, leftX));
  return;
}

void assignMovement(int movement[]) {
  for (int i = 0; i < 4; i++) {
    currentMovement[i] = movement[i];
  }
}

float getScale(int ly, int lx) {
  float higher = (abs(ly) > abs(lx)) ? (float)ly : (float)lx;
  return (higher / 100.0);
}

int scaleSpeed(float scale, int value) {
  return (scale * value);
}

void moveMotors(int movement[], float scale) {
  Motor.speed(1, MOTOR1, scaleSpeed(scale, movement[0]));
  Motor.speed(1, MOTOR2, scaleSpeed(scale, movement[1]));
  Motor.speed(0, MOTOR2, scaleSpeed(scale, movement[2]));
  Motor.speed(0, MOTOR1, scaleSpeed(scale, movement[3]));
}

void stopMotors(){
  Motor.speed(1, MOTOR1, 0);
  Motor.speed(1, MOTOR2, 0);
  Motor.speed(0, MOTOR2, 0);
  Motor.speed(0, MOTOR1, 0);
}

void printState() {
  Serial.println("[" + String(leftY) + ":" + String(leftX) + ":" + String(isLeftJoypadDown) + ":" + String(rightY) + ":" + String(rightX) + ":" + String(isRightJoypadDown) + "]");
}

int getBounds(int value) {
  if ((-7 < value) && (value < 5)) {
    return 0;
  }
  return value;
}

int mapValue(int value) {
  return map(value, 0, 1023, -100, 100);
}

void assignValues() {
  leftY = getBounds(mapValue(inputMap[0])), leftX = getBounds(mapValue(inputMap[1]));
  rightY = getBounds(mapValue(inputMap[3])), rightX = getBounds(mapValue(inputMap[4]));
  isLeftJoypadDown = (inputMap[2] == 1) ? false : true;
  isRightJoypadDown = (inputMap[5] == 1) ? false : true;
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
