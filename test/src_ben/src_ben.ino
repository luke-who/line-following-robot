#include "motors.h"
#include "ir_sensors.h"
#include "encoders.h"
#include "kinematics.h"

#define STATE_CALIBRATING 0
#define STATE_LINE_FOLLOWING 1
#define STATE_ODOMETRY_TEST 2
#define STATE_GO_HOME 3
#define STATE_UNKOWN 4

#define BASE_SPEED 20
#define SPEED_GAIN (3*BASE_SPEED)
#define NUM_TARGETS 4

Motors_c motors;
IRSensors_c irSensors(0.5);
Kinematics_c kinematics(&encoders, &motors);
int current_state;
unsigned long state_start_time;
float line_error;
double point_error;
int i = 0;
double x_targets[4] = {150, 150, -150, -150};
double y_targets[4] = {150, -150, -150, 150};

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("Ready.");
  motors.init();
  irSensors.init();
  encoders.init();
  current_state = STATE_CALIBRATING;
  state_start_time = millis();
}

void flushSerial() {
  Serial.println("");
  Serial.flush();
}

void loop() {

  switch (current_state) {

    case STATE_CALIBRATING:
      irSensors.readIRSensors();
      irSensors.calibIRSensors();
      motors.setPower(20, 20);
      if (millis() - state_start_time > 1000) {
        motors.setPower(0, 0);
        delay(2000);
        current_state = STATE_LINE_FOLLOWING;
      }
      break;

    case STATE_LINE_FOLLOWING:
      kinematics.update();
      irSensors.readIRSensors();
      line_error = irSensors.line_error();
      motors.setPower(round(BASE_SPEED - line_error * SPEED_GAIN), round(BASE_SPEED + line_error * SPEED_GAIN));
      if (kinematics.close_to(550, 80)) {
        motors.setPower(0, 0);
        current_state = STATE_GO_HOME;
      }
      break;

    case STATE_GO_HOME:
      kinematics.update();
      point_error = kinematics.point_to(-50, 10);
      if (point_error > 0.1) {
        motors.setPower(25, -25);
      } else if (point_error < -0.1) {
        motors.setPower(-25, 25);
      } else {
        motors.setPower(20, 20);
      }
      if (kinematics.close_to(0, 0)) {
        motors.setPower(0, 0);
        current_state = STATE_UNKOWN;
      }
      break;

    case STATE_ODOMETRY_TEST:
      kinematics.update();
      point_error = kinematics.point_to(x_targets[i], y_targets[i]);
      if (point_error > 0.1) {
        motors.setPower(25, -25);
      } else if (point_error < -0.1) {
        motors.setPower(-25, 25);
      } else {
        motors.setPower(0, 0);
      }
      if (millis() - state_start_time > 2000) {
        i++;
        state_start_time = millis();
        if (i >= NUM_TARGETS) {
          motors.setPower(0, 0);
          current_state = STATE_UNKOWN;
        }
      }
      //      weird double overflow.
      //      if (kinematics.theta_global >= TWO_PI) {
      //        motors.setPower(0, 0);
      //        current_state = STATE_UNKOWN;
      //      } else {
      //        motors.setPower(20, -20);
      //      }
      break;

    default:
      delay(500);
      break;
  }
}
