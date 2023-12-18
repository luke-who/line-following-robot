#include "motors.h"
#include "ir_sensors.h"
#include "encoders.h"
#include "kinematics.h"


Motors_c motors;
IRSensors_c irSensors(0.5);
Kinematics_c kinematics(&encoders, &motors);

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("Ready.");
  motors.init();
  irSensors.init();
  encoders.init();
}

void loop() {
    kinematics.update();
    Serial.println("X_I:"+String(kinematics.x_global)+",Y_I:"+String(kinematics.y_global)+",theta_I:"+String(kinematics.theta_global));

}
