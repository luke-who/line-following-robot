#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN 9
#define R_DIR_PIN 15
#define FWD LOW
#define REV HIGH
#define MAXIMUM_SPEED 100

class Motors_c {
  public:
    Motors_c() {
    }
    void init(){
      pinMode(L_PWM_PIN, OUTPUT);
      pinMode(L_DIR_PIN, OUTPUT);
      pinMode(R_PWM_PIN, OUTPUT);
      pinMode(R_DIR_PIN, OUTPUT);
    }
    void setPower(int lPower, int rPower){
      if (lPower < 0){
        lPower = -lPower;
        digitalWrite(L_DIR_PIN, REV);
      } else {
        digitalWrite(L_DIR_PIN, FWD);
      }
      if (lPower > MAXIMUM_SPEED) {
        lPower = MAXIMUM_SPEED;
      }
      analogWrite(L_PWM_PIN, lPower);
      if (rPower < 0){
        rPower = -rPower;
        digitalWrite(R_DIR_PIN, REV);
      } else {
        digitalWrite(R_DIR_PIN, FWD);
      }
      if (rPower > MAXIMUM_SPEED) {
        rPower = MAXIMUM_SPEED;
      }
      analogWrite(R_PWM_PIN, rPower);
    }
}; 
