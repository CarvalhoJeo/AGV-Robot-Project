// Motor A (left)
#define motorA_pwm 11
#define motorA_fwd 12
#define motorA_bcw 13

// Motor B (right)
#define motorB_pwm 5
#define motorB_fwd 6
#define motorB_bcw 7

#define left_IR  A0
#define right_IR A1

#define line_counter_IR A3

#define KP         0.5f

enum State {
  FOLLOW_LINE,
  STOP,
  TURN,
  TEST
};

enum State actual_state = FOLLOW_LINE;

void setup() {
  pinMode(motorA_pwm, OUTPUT);
  pinMode(motorA_fwd, OUTPUT);
  pinMode(motorA_bcw, OUTPUT);

  pinMode(motorB_pwm, OUTPUT);
  pinMode(motorB_fwd, OUTPUT);
  pinMode(motorB_bcw, OUTPUT);
}

void loop() {
  switch (actual_state) {
    case FOLLOW_LINE:
      followLine();
      break;

    case STOP:
      stopMotors();
      break;

    case TURN:

      break;
    case TEST:
      driveForward(200);
      delay(2000);
      driveBackward(200);
      break;
  }
}

void setMotorA(int speed) {
  if (speed >= 0) {
    analogWrite(motorA_pwm, speed);
    digitalWrite(motorA_fwd, HIGH);
    digitalWrite(motorA_bcw, LOW);
  } else {
    analogWrite(motorA_pwm, -speed);
    digitalWrite(motorA_fwd, LOW);
    digitalWrite(motorA_bcw, HIGH);
  }
}

void setMotorB(int speed) {
  if (speed >= 0) {
    analogWrite(motorB_pwm, speed);
    digitalWrite(motorB_fwd, HIGH);
    digitalWrite(motorB_bcw, LOW);
  } else {
    analogWrite(motorB_pwm, -speed);
    digitalWrite(motorB_fwd, LOW);
    digitalWrite(motorB_bcw, HIGH);
  }
}

void driveForward(int speed) {
  setMotorA(speed);
  setMotorB(speed);
}

void driveBackward(int speed) {
  setMotorA(-speed);
  setMotorB(-speed);
}

void stopMotors() {
  setMotorA(0);
  setMotorB(0);
}

void followLine() {
  int leftVal  = analogRead(left_IR);
  int rightVal = analogRead(right_IR);

  int error      = leftVal - rightVal;
  int correction = (int)(KP * error);

  int speedA = constrain(150 + correction, -255, 255);
  int speedB = constrain(150 - correction, -255, 255);

  setMotorA(speedA);
  setMotorB(speedB);
}
