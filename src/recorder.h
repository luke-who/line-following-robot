#ifndef _RECORDER_H
#define _RECORDER_H

#define DATA_SIZE 300

class Recorder_c {
  private:
  float data[DATA_SIZE];
  unsigned int index = 0;
  float point_per_dist;

  public:
  void init(float limit_i) {
    for(int i; i < DATA_SIZE; i++){
      data[i] = 0.f;
    }
    point_per_dist = (DATA_SIZE-1)/limit_i;
  }

  void record(float input, float dist) {
    index = round(dist * point_per_dist);
    if (index < DATA_SIZE) {
      data[index] = input;
    }
//    Serial.print("dist:" + String(dist)); 
//    Serial.println(", index:" + String(index));
  }

  void output() {
    for(int i; i < DATA_SIZE; i++){
      Serial.print("data[" + String(i) + "]:");
      Serial.println(data[i], 8);
    }
  }
};

#endif
