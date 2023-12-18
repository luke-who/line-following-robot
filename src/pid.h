#ifndef _PID_H
#define _PID_H

#define BASE_SPEED 30
#define P_GAIN (2*BASE_SPEED)
#define D_GAIN (4*BASE_SPEED)
#define I_GAIN 0.012

class PID_c {

  private:
  Motors_c *motors;
  unsigned long ms_last_ts;
  unsigned long delta_t;
  float last_error;
  float error_diff;
  float error_int;

  public:
  float feedback;
  PID_c(Motors_c *motors_i) {
    motors = motors_i;
  } 

  void init() {
    ms_last_ts = millis();
    last_error = 0.f;
    error_diff = 0.f;
    error_int = 0.f;
  }

  update(float line_error) {

    delta_t = millis()-ms_last_ts;
    ms_last_ts = millis();
    if (delta_t != 0.f) {
      error_diff = (line_error - last_error) / float(delta_t); 
    }
    last_error = line_error;

    error_int = error_int + line_error * float(delta_t);
    feedback = line_error * P_GAIN + D_GAIN * error_diff + I_GAIN * error_int;
//    Serial.println("p:" + String(line_error * SPEED_GAIN) + ", d:" + String(D_GAIN * error_diff) + ", i:" + String(I_GAIN * error_int) + ", t:" + String(feedback));
    motors->setPower(round(BASE_SPEED - feedback), round(BASE_SPEED + feedback));
  }
};

#endif
