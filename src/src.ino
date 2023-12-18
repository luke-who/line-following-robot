#include "motors.h"
#include "ir_sensors.h"
#include "encoders.h"
#include "kinematics.h"
#include "recorder.h"
#include "pid.h"

#define MAX_DISTANCE 485

#define STATE_CALIBRATION 0
#define STATE_LINE_FOLLOWING 1
#define STATE_DONE 2
#define STATE_UNKNOWN -1

float line_error;
int current_state = STATE_CALIBRATION;
unsigned long state_timer;

Motors_c motors;
Kinematics_c kinematics(&encoders);
IRSensors_c irSensors;
Recorder_c recorder;
PID_c pid(&motors);

// put your setup code here, to run once:
void setup() {
  
  encoders.init();
  kinematics.initialise();
  motors.init();
  irSensors.init();
  recorder.init(MAX_DISTANCE);
  pid.init();

  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");
}


void loop() {
    kinematics.update();
  
    switch (current_state) {
 
      case STATE_CALIBRATION:
        irSensors.readIRSensors();
        irSensors.calibIRSensors();
        motors.setPower(-20,20);
        if (kinematics.theta_I > TWO_PI) {
          current_state = STATE_LINE_FOLLOWING;
          kinematics.initialise();
        }
        break;
        
      case STATE_LINE_FOLLOWING:
        irSensors.readIRSensors();
        line_error = irSensors.line_error();
        pid.update(line_error);
        recorder.record(line_error, kinematics.X_I);
        kinematics.theta_I = 0;
        if (kinematics.X_I >= MAX_DISTANCE) {
          current_state = STATE_DONE;
          state_timer = millis();
        }
        break;

      case STATE_DONE:
        motors.setPower(0,0);
        Serial.end();
        if (millis() - state_timer >= 10000){ 
          Serial.begin(9600);
          recorder.output();
        }
        
      default:
        motors.setPower(0,0);
        break;
    }
}
