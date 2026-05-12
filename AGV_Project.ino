#define motorA_pwm 11
#define motorA_fwd 13
#define motorA_bcw 12

#define motorB_pwm 5
#define motorB_fwd 7
#define motorB_bcw 6

#define left_IR  A0
#define right_IR A1

#define line_counter_IR A3
#define front_IR        2    // porta sensor de anti-colisão

#define BLACK_THRESHOLD 200
#define BASE_SPEED      120
#define CORRECTION      50 //velocidade da curva

enum State {
  FOLLOW_LINE,
  STOP,
  TURN,
  TEST
};

enum State actual_state = FOLLOW_LINE;

volatile bool obstacle_flag = false;

void onObstacle() {
  if (digitalRead(front_IR) == LOW) {
    obstacle_flag = true;
  } else {
    obstacle_flag = false;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(motorA_pwm, OUTPUT);
  pinMode(motorA_fwd, OUTPUT);
  pinMode(motorA_bcw, OUTPUT);

  pinMode(motorB_pwm, OUTPUT);
  pinMode(motorB_fwd, OUTPUT);
  pinMode(motorB_bcw, OUTPUT);

  pinMode(front_IR, INPUT);
  attachInterrupt(digitalPinToInterrupt(front_IR), onObstacle, CHANGE);
}

void loop() {
  switch (actual_state) {
    case FOLLOW_LINE:
      if (obstacle_flag) {
        stopMotors();
        actual_state = STOP;
      } else {
        followLine();
      }
      break;

    case STOP:
      if (!obstacle_flag) {
        actual_state = FOLLOW_LINE;
      }
      break;

    case TURN:

      break;
    case TEST:
      driveForward(200);
      delay(2000);
      stopMotors();
      delay(1000);
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
  bool leftOnLine  = analogRead(left_IR)  > BLACK_THRESHOLD;
  bool rightOnLine = analogRead(right_IR) > BLACK_THRESHOLD;

  int correction = 0;
  if      (leftOnLine  && !rightOnLine) correction =  CORRECTION;
  else if (rightOnLine && !leftOnLine)  correction = -CORRECTION;

  setMotorA(constrain(BASE_SPEED - correction, -255, 255));
  setMotorB(constrain(BASE_SPEED + correction, -255, 255));
}
