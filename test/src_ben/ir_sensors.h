#define EMIT 11
#define IR_CL A0
#define IR_CEN A2
#define IR_CR A3
#define IR_SENSORS 3
#define IR_SENSORS_LIST {IR_CL, IR_CEN, IR_CR}
#define INDEX_LEFT 0
#define INDEX_CENTER 1
#define INDEX_RIGHT 2

class IRSensors_c {
  public:
    IRSensors_c(float input_max) {
      max_ratio = input_max;
    }
    void init() {
      pinMode(EMIT, OUTPUT);
      digitalWrite(EMIT, HIGH);
      for ( int i = 0; i < IR_SENSORS; i++ ) {
        sensor_min[i] = 10000;
        sensor_max[i] = 0;
      }
    }
    void calibIRSensors() {
      for ( int i = 0; i < IR_SENSORS; i++ ) {
        if (sensor_val[i] < sensor_min[i]) {
          sensor_min[i] = sensor_val[i];
        }
        if (sensor_val[i] > sensor_max[i]) {
          sensor_max[i] = sensor_val[i];
        }
      }
    }
    void readIRSensors() {
      for ( int i = 0; i < IR_SENSORS; i++ ) {
        done[i] = false;
        setupIRSensor(sensor_pins[i]);
      }
      delayMicroseconds(10);
      for ( int i = 0; i < IR_SENSORS; i++ ) {
        pinMode(sensor_pins[i], INPUT);
      }
      start_time = micros();
      while (true) {
        done_t = true;
        for ( int i = 0; i < IR_SENSORS; i++ ) {
          if (!(digitalRead(sensor_pins[i]) == HIGH)) {
            end_time = micros();
            if (!done[i]) {
              done[i] = true;
              sensor_val[i] = end_time - start_time;
            }
          } else {
            done_t = false;
          }
        }
        if (done_t) {
          break;
        }
      }
    }
    bool too_left() {
      return sensor_val[INDEX_LEFT] > (sensor_min[INDEX_LEFT] + max_ratio * float(sensor_max[INDEX_LEFT] - sensor_min[INDEX_LEFT]));
    }
    bool too_right() {
      return sensor_val[INDEX_RIGHT] > (sensor_min[INDEX_RIGHT] + max_ratio * float(sensor_max[INDEX_RIGHT] - sensor_min[INDEX_RIGHT]));
    }
    bool too_center() {
      return sensor_val[INDEX_CENTER] > (sensor_min[INDEX_CENTER] + max_ratio * float(sensor_max[INDEX_CENTER] - sensor_min[INDEX_CENTER]));
    }
    float sensor_activation(int index) {
      return float(long(sensor_val[index]) - long(sensor_min[index])) / float(long(sensor_max[index]) - long(sensor_min[index]));
    }
    float line_error() {
      float left_activation = sensor_activation(INDEX_LEFT);
      float center_activation = sensor_activation(INDEX_CENTER);
      float right_activation = sensor_activation(INDEX_RIGHT);
      float toatal_activation = left_activation + center_activation + right_activation;
      left_activation = left_activation / toatal_activation;
//      center_activation = center_activation / toatal_activation;
      right_activation = right_activation / toatal_activation;
      if (toatal_activation < 0.05) {
        return 0;
      } else {
        return left_activation - right_activation;
      }
    }

  private:
    uint8_t sensor_pins[IR_SENSORS] = IR_SENSORS_LIST;
    unsigned long sensor_val[IR_SENSORS];
    unsigned long sensor_min[IR_SENSORS];
    unsigned long sensor_max[IR_SENSORS];
    bool done[IR_SENSORS];
    bool done_t;
    unsigned long start_time;
    unsigned long end_time;
    float max_ratio;

    void setupIRSensor(uint8_t pin) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
    }
};
