// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _MOTORS_H
#define _MOTORS_H

# define L_PWM_PIN 10
# define L_DIR_PIN 16
# define R_PWM_PIN 9
# define R_DIR_PIN 15

# define FWD LOW
# define REV HIGH

# define MAX_PWM 255.f

// Class to operate the motor(s).
class Motors_c {
  public:
      
    bool halted = true;
//    int incr = 0;
    // Constructor, must exist.
    Motors_c() {

    } 

    // Use this function to 
    // initialise the pins and 
    // state of your motor(s).
    void initialise() {
      // Set all the motor pins as outputs.
      pinMode(L_PWM_PIN,OUTPUT);
      pinMode(L_DIR_PIN,OUTPUT);
      pinMode(R_PWM_PIN,OUTPUT);
      pinMode(R_DIR_PIN,OUTPUT);
      // Set initial direction (HIGH/LOW) for the direction pins.
      digitalWrite(L_DIR_PIN,FWD);
      digitalWrite(R_DIR_PIN,FWD);
      // Set initial values for the PWM Pins.
      analogWrite(L_PWM_PIN,0);
      analogWrite(R_PWM_PIN,0);
      
      // Start serial, send debug text.
      Serial.begin(9600);
      delay(1000);
      Serial.println("***RESET***");
    }

    /*
     * Sets the power of a motor using analogWrite().
     * This function sets direction and PWM (power).
     * This function catches all errors of input PWM.
     *  inputs: 
     *     pwm   accepts negative, 0 and positve
     *           values.  Sign of value used to set
     *           the direction of the motor.  Values
     *           are limited in range [ ??? : ??? ].
     *           Magnitude used to set analogWrite().
     */
    void setMotorValues(float value_l,float value_r){
        if (value_l<=0 && value_r<=0){
          digitalWrite(L_DIR_PIN,REV);
          digitalWrite(R_DIR_PIN,REV);
//          value_l=-value_l;
//          value_r=-value_r;
        }else if (value_l>=0 && value_r>=0) {
          digitalWrite(L_DIR_PIN,FWD);
          digitalWrite(R_DIR_PIN,FWD);
        }else if (value_l>=0 && value_r<=0) {
          digitalWrite(L_DIR_PIN,FWD);
          digitalWrite(R_DIR_PIN,REV);
//          value_r=-value_r;
        }else if (value_l<=0 && value_r>=0) {
          digitalWrite(L_DIR_PIN,REV);
          digitalWrite(R_DIR_PIN,FWD);
//          value_l=-value_l;   
        } 
        
        value_l = (abs(value_l)>=MAX_PWM) ? MAX_PWM : abs(value_l);
        value_r = (abs(value_r)>=MAX_PWM) ? MAX_PWM : abs(value_r);
        
        analogWrite(L_PWM_PIN,value_l);
        analogWrite(R_PWM_PIN,value_r);
      }
    
    void forward(int value_l, int value_r){
//      Set the direction forward
      digitalWrite(L_DIR_PIN,FWD);
      digitalWrite(R_DIR_PIN,FWD);
      analogWrite(L_PWM_PIN,value_l);
      analogWrite(R_PWM_PIN,value_r);
    }

    void backward(int value_l, int value_r){
//      Set the direction backward
      digitalWrite(L_DIR_PIN,REV);
      digitalWrite(R_DIR_PIN,REV);
      analogWrite(L_PWM_PIN,value_l);
      analogWrite(R_PWM_PIN,value_r);
    }
    
    void turn_left(int value_l, int value_r){
      digitalWrite(L_DIR_PIN,FWD);
      digitalWrite(R_DIR_PIN,FWD);
      analogWrite(L_PWM_PIN,value_l);
      analogWrite(R_PWM_PIN,value_r);
    }
    
    void turn_right(int value_l, int value_r){
      digitalWrite(L_DIR_PIN,FWD);
      digitalWrite(R_DIR_PIN,FWD);
      analogWrite(L_PWM_PIN,value_l);
      analogWrite(R_PWM_PIN,value_r);
    }
    
    void drive_arc(){
      digitalWrite(L_DIR_PIN,FWD);
      digitalWrite(R_DIR_PIN,FWD);
      analogWrite(L_PWM_PIN,13);
      analogWrite(R_PWM_PIN,20);
    }
    /*  Halt the robot  */
    void halt(){
      // Example to stop the robot indefinitely.
      // Only a hard reset will break the loop.
//      while(1) {
        analogWrite(L_PWM_PIN,0);
        analogWrite(R_PWM_PIN,0);
        Serial.println("Program Halted");
//        delay(500);
//      }
    }
    
};



#endif
