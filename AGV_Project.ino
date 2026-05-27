#define motorA_pwm 11
#define motorA_fwd 13
#define motorA_bcw 12

#define motorB_pwm 5
#define motorB_fwd 7
#define motorB_bcw 6

#define left_IR  A0
#define right_IR A1

#define line_counter_IR 3
#define front_IR        2

#define BLACK_THRESHOLD_LEFT 250
#define BLACK_THRESHOLD_RIGHT 150

#define BASE_SPEED      250
#define CORRECTION      150
#define TURN_SPEED      200

enum Command { GO_STRAIGHT, GO_LEFT, GO_RIGHT, FINISH };

Command traj[] = {
  GO_STRAIGHT,
  GO_STRAIGHT,
  GO_LEFT,
  GO_STRAIGHT,
  GO_STRAIGHT,
  GO_STRAIGHT,
  FINISH
};

const int TRAJ_LENGTH = sizeof(traj) / sizeof(traj[0]);

enum State {
  FOLLOW_LINE,
  STOP,
  TURN_LEFT,
  TURN_RIGHT,
  TEST
};

volatile enum State actual_state = FOLLOW_LINE;

volatile bool obstacle_flag = false;

volatile int indexTraj = 0;

volatile bool turned = false;

bool lastLineCounter = LOW;

void onObstacle() {
  if (digitalRead(front_IR) == LOW) {
    obstacle_flag = true;
  } else {
    obstacle_flag = false;
  }
}

void changeState() {
  if (actual_state != FOLLOW_LINE || indexTraj >= TRAJ_LENGTH) {
    return;
  }

  if (turned) {
    turned = false;
    return;
  }

  switch (traj[indexTraj]) {
    case GO_STRAIGHT:
      actual_state = FOLLOW_LINE;
      break;
    case GO_LEFT:
      actual_state = TURN_LEFT;
      break;
    case GO_RIGHT:
      actual_state = TURN_RIGHT;
      break;
    case FINISH:
      actual_state = STOP;
      return;
  }
  indexTraj++;
}

void setup() {
  Serial.begin(9600);
  pinMode(motorA_pwm, OUTPUT);
  pinMode(motorA_fwd, OUTPUT);
  pinMode(motorA_bcw, OUTPUT);

  pinMode(motorB_pwm, OUTPUT);
  pinMode(motorB_fwd, OUTPUT);
  pinMode(motorB_bcw, OUTPUT);

  pinMode(line_counter_IR, INPUT);
  pinMode(front_IR, INPUT);
  attachInterrupt(digitalPinToInterrupt(front_IR), onObstacle, CHANGE);
}

void loop() {
  bool currentLineCounter = digitalRead(line_counter_IR);
  Serial.println(currentLineCounter);
  if (currentLineCounter == HIGH && lastLineCounter == LOW) {
    changeState();
  }
  lastLineCounter = currentLineCounter;

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
      stopMotors();
      if (!obstacle_flag && traj[indexTraj] != FINISH) {
        actual_state = FOLLOW_LINE;
      }
      break;

    case TURN_LEFT:
      spinLeft();
      actual_state = FOLLOW_LINE;
      break;

    case TURN_RIGHT:
      spinRight();
      actual_state = FOLLOW_LINE;
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
  bool leftOnLine  = analogRead(left_IR)  > BLACK_THRESHOLD_LEFT;
  bool rightOnLine = analogRead(right_IR) > BLACK_THRESHOLD_RIGHT;

  int correction = 0;
  if (leftOnLine && !rightOnLine) {
    correction = +CORRECTION;
  } else if (rightOnLine && !leftOnLine) {
    correction = -CORRECTION;
  }

  setMotorA(constrain(BASE_SPEED - correction, -255, 255));
  setMotorB(constrain(BASE_SPEED + correction, -255, 255));
}

void spinRight() {
  stopMotors();
  delay(50);
  setMotorA(TURN_SPEED);
  setMotorB(-TURN_SPEED);
  delay(80);
  while (analogRead(left_IR) <= BLACK_THRESHOLD_LEFT) {}
  while (analogRead(right_IR) <= BLACK_THRESHOLD_RIGHT) {} 
  delay(100);
  turned = true;
  stopMotors();
  delay(50);
}

void spinLeft() {
  stopMotors();
  delay(50);
  setMotorA(-TURN_SPEED);
  setMotorB(TURN_SPEED);
  delay(80);
  while (analogRead(right_IR) <= BLACK_THRESHOLD_RIGHT) {}
  while (analogRead(left_IR) <= BLACK_THRESHOLD_LEFT) {}
  delay(100);
  turned = true;
  stopMotors();
  delay(50);
}
