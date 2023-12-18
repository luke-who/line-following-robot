#include "motors.h"
#include "linesensor.h"
#include "encoders.h"
#include "kinematics.h"
#include "pid.h"

#define GAIN_PWM 50 // What is a sensible max pwm for rotation?
#define MIN_PWM 50

#define LED_PIN 13  // Pin to activate the orange LED
#define BUZZER_PIN 6
//-------------------------------//
// Declare your different possible states here by enumerating them.
// Remember, a #define works like a find and replace in code.
#define STATE_INITIAL 0
#define STATE_DRIVE_FORWARDS   1
#define STATE_FOUND_LINE  2
#define STATE_LINE_FOLLOWING 3
#define STATE_UNKNOWN -1

long last_count_l;
long last_count_r;

boolean led_state;  // Variable to "remember" the state of the LED, and toggle it.
int buzz_times = 0;
bool timeout=false;
bool calibration_complete=false;
int calibration_end_t;
// This will hold which state your robot is in. It will receive 
// the numbers set above, but your code will be readable because 
// strings used in the #define's
int state;

//Create an instance of the linesensor class
LineSensor_c line_sensors;
//Create an instance of the Motors class
Motors_c motors;
//Create an instance of the Kinematics class
Kinematics_c kinematics;

float sensor_min_max[NB_LS_PINS][2]; //2 dimensional array to store each sensor's min&max readings
float c[NB_LS_PINS]; //conditioned measurement of sensors

// put your setup code here, to run once:
void setup() {
  /* Insert necessary initialising here.  e.g classes, pin modes 
     and states, Serial.begin() */
  // Set LED pin as an output
  pinMode( LED_PIN, OUTPUT );
  // Set initial state of the LED
  led_state = false;
  pinMode(BUZZER_PIN,OUTPUT);
  
  // Setup encoders when the robot is 
  // powered on. 
  setupEncoder0();
  setupEncoder1();

  last_count_l = 0;
  last_count_r = 0;
  kinematics.initialise();
  line_sensors.initialise();
  motors.initialise();

  // Start serial, send debug text.
  Serial.begin(9600);
  delay(1000);
  Serial.println("***RESET***");

  // Set system state to intial before robot begins to operate.
  state = STATE_INITIAL;

}


// put your main code here, to run repeatedly:
void loop() {
//  line_sensors.printSensorsReadings();
//   Always call a function to update the state
//   variable.  loop() should be returning quickly.
//   flow control is managed in updateState()
  updateState();

  // This is the basic structure for a FSM  Based on the value 
  // of "state" variable, run appropriate code for robot behaviour.
  if( state == STATE_INITIAL ) {

          calibration_setup();     
  } else if( state == STATE_DRIVE_FORWARDS ) {
          if ((millis()-calibration_end_t)>5000){//timeout if no line found after 5s
            timeout=true;
          }
          driveForwards();     

  } else if( state == STATE_FOUND_LINE ) {
          foundLineBeeps(1);

  } else if( state == STATE_LINE_FOLLOWING ) {
          lineFollowingBehaviour();
  } else {

          // You can catch situations where the robot
          // attempts to move into an unknown state.
          Serial.print("System Error, Unknown state: ");
          Serial.println( state );

          // You might want to develop codes to represent
          // what type of problem was encountered.
          Serial.print("Error code: ");
          Serial.println( String(-1) );

          // These functions might be useful to write.
          motors.halt();
//          stopRobot();
//          signalError();
  }

}

// A function to update the system variables, and to
// cause transitions between system states. 
void updateState() {

  switch (state) {
    case STATE_INITIAL:
          if (calibration_complete==false){
            state=STATE_INITIAL;
          }else{
            state=STATE_DRIVE_FORWARDS;
          }
          break;
    case STATE_DRIVE_FORWARDS:
          if (found_line()==false){
            if (timeout==true){
              Serial.println("current:"+String(millis())+",calibration_end_t:"+String(calibration_end_t));
              state=STATE_UNKNOWN;
            }else{
              state=STATE_DRIVE_FORWARDS;
            }            
          }else{
            state=STATE_FOUND_LINE;
          }
    
          break;
    case STATE_FOUND_LINE:
          state = STATE_LINE_FOLLOWING;
          break;
    case STATE_LINE_FOLLOWING:
          state = STATE_LINE_FOLLOWING;
          break;
    case STATE_UNKNOWN:
          state = STATE_UNKNOWN;
          break;
    default:
          state=STATE_INITIAL;
          break;
  }
  
}



// write this function to have your
// robot beep 5 times, across a total
// of 5 seconds.
//void intialisingBeeps() {
//  
//}



// Write code that will command your
// robot to drive forwards straight
// until it detects the course line.
void driveForwards() {
  motors.forward(20,20);
}



// Write code that will deactivate all
// motors, and beep once for one second,
// then do nothing.
void foundLineBeeps(int toggle_duration) {
  if (buzz_times<1){
    motors.halt();
    analogWrite(BUZZER_PIN,20);
    delay(toggle_duration*1000);                       // wait for some millisecond
    analogWrite(BUZZER_PIN,0);
    buzz_times+=1;
  }
}






//-----------------------------------//

void calibration_setup(){
  // Initialise sensor readings
  line_sensors.parallelLineSensorRead();
  for (int which = 0; which < NB_LS_PINS; which++ ) {
    sensor_min_max[which][0]=line_sensors.ls_reading[which];
    sensor_min_max[which][1]=line_sensors.ls_reading[which];
  }

  //rotate to collect samples
  int n_samples = 0;
//  kinematics.theta_I=4.84 turns 360 degrees 
  while (kinematics.theta_I<4.84 && kinematics.theta_I>=0) {
    //check if the wheels have been rotated
    kinematics.phi_l=last_count_l-count_encdr_l;
    kinematics.phi_r=last_count_r-count_encdr_r;
    kinematics.update();
    motors.setMotorValues(-20,20);
    
    line_sensors.parallelLineSensorRead();

    for (int which = 0; which < NB_LS_PINS; which++ ) {
      sensor_min_max[which][0]=min(sensor_min_max[which][0],line_sensors.ls_reading[which]);
      sensor_min_max[which][1]=max(sensor_min_max[which][1],line_sensors.ls_reading[which]);
    }

      n_samples+=1;
//    line_sensors.printSensorsReadings();
    last_count_l=count_encdr_l;
    last_count_r=count_encdr_r;
    delay(50);
  }
    motors.halt();
    calibration_complete=true;
  //Blink the LED for 2 seconds to indicate completion of calibration
  blink_led(2);
  calibration_end_t=millis();
  Serial.println("***CALIBRATION COMPLETE***");
  for (int which = 0; which < NB_LS_PINS; which++ ) {
    Serial.println(String(sensor_min_max[which][0])+','+String(sensor_min_max[which][1])+','+"n_samples:"+String(n_samples));
  }


}

/*A function to calculate conditioined measurement for sensors*/
void conditioined_measure(){
  float m[NB_LS_PINS]; //unconditioned, or raw measurement of sensors
  float bias[NB_LS_PINS]; //sensor offset bias
  float S[NB_LS_PINS]; //scaling factor
  for (int which = 0; which < NB_LS_PINS; which++ ) {
    line_sensors.parallelLineSensorRead();
    m[which] = line_sensors.ls_reading[which];
    bias[which]=sensor_min_max[which][0];
    S[which] = 1/(sensor_min_max[which][1]-sensor_min_max[which][0]);
    c[which] = (m[which]-bias[which])*S[which];

//    Serial.print(String(c[which])+",");
  }
//  Serial.print("\n");


}

/* A function to return an error signal representative
   of the line placement under the ground sensor.
 !! Be careful with mixed variable types!!
*/
float getLineError() {
  float e_line;
  float w_left;
  float w_right;

  // Get latest line sensor readings.
  // You may need to call an updateLineSensor()
  // function from loop before hand - it depends
  // on your implementation.
  conditioined_measure();
  // Sum ground sensor activation
  float sum = 0;
  for (int i = 0; i < NB_LS_PINS; i++) {
    sum+=c[i];
//    Serial.print(String(c[i]) + ',');
  }
//  Serial.println("sum = "+String(sum));

  // Normalise individual sensor readings 
  // against sum
  float L_left = c[0]/sum;
  float L_centre = c[1]/sum;
  float L_right = c[2]/sum;
//  Serial.println("L_left:"+String(L_left)+" L_centre:"+String(L_centre)+" L_right: "+String(L_right)+" sum="+String(L_left+L_centre+L_right));
  // Calculated error signal
  w_right = L_right+(L_centre*0.5);
  w_left  = L_left+(L_centre*0.5);
  e_line  = w_left-w_right;

//  Serial.println("e_line:"+String(e_line));
  // Return result
  return e_line;
}

void lineFollowingBehaviour() {
/*Implementation 2: weighted measurement*/
  // Get the line error
  float e_line;
  e_line = getLineError();

  // GAIN_PWM is scaled by e_line [ -1.0 : +1.0 ]
  float weighted_e;
  weighted_e = GAIN_PWM * e_line;
  // Set motor values.
  // Assuming you have written a function to command
  // your motors.
  // What does "0 -" and "0 +" achieve here?
//  if (weighted_e<=MIN_PWM && weighted_e>=-MIN_PWM){
//    motors.setMotorValues( MIN_PWM, MIN_PWM );
//  }
//  if (weighted_e<-MIN_PWM) {
//    weighted_e=-MIN_PWM;
//    motors.setMotorValues( (0 - weighted_e), (0 + weighted_e) );
//  }else if (weighted_e>MIN_PWM) {
//    weighted_e=MIN_PWM;
//    motors.setMotorValues( (0 - weighted_e), (0 + weighted_e) );
//  }
   if (millis()>19000 && millis()<21000) {
     motors.setMotorValues(MIN_PWM,MIN_PWM);
   }else{
     if (weighted_e>0 && weighted_e<=MIN_PWM){ //turn left
       motors.setMotorValues( MIN_PWM, (MIN_PWM + weighted_e) );
     }else if (weighted_e<0 && weighted_e>=-MIN_PWM){ //turn right
      motors.setMotorValues( (MIN_PWM - weighted_e), MIN_PWM );
     }else if (weighted_e<-MIN_PWM){ //rotate right
      motors.setMotorValues( (0 + MIN_PWM), (0 - MIN_PWM));
     }else if (weighted_e>MIN_PWM){ //rotate left
      motors.setMotorValues( (0 - MIN_PWM), (0 + MIN_PWM));
     }else { //drive forward
       motors.setMotorValues(MIN_PWM,MIN_PWM);
     }
   }
//  Serial.println(weighted_e);
  
}

bool found_line(){
  bool found_line=false;
  line_sensors.parallelLineSensorRead();
  for (int which = 0; which < NB_LS_PINS; which++ ){
    found_line = found_line || (line_sensors.ls_reading[which]>=1000)?true:false ;
  }
//  if ((line_sensors.ls_reading[0]>=1000) ||
//      (line_sensors.ls_reading[1]>=1000) ||
//      (line_sensors.ls_reading[2]>=1000) ){
//        found_line=true;
//      }
  return found_line;
}
/*Blink the led every 1 second*/
void blink_led(int second){
  unsigned long led_t = second*1000;
  unsigned long start_t=millis();
  while ((millis()-start_t)<=led_t){
    if( led_state == true ) {
      led_state = false;
    } else {
      led_state = true;
      Serial.println("Blink");
    }
  
    // We use the variable to set the
    // debug led on or off on the 3Pi+
    digitalWrite( LED_PIN, led_state );
  
    delay(500);
  }
  digitalWrite( LED_PIN, 0);

}


float calculate_avg(float *arr,int len){
  float sum = 0.f;
  float avg = 0.f;
  
  for (int i = 0; i < len; ++i)
      sum += arr[i]; 

  avg = sum/float(len);

  return avg;
}

/*quick sort*/
// qsort requires you to create a sort function
int sort_desc(const void *cmp1, const void *cmp2) {
  // Need to cast the void * to int *
  float a = *((float *)cmp1);
  float b = *((float *)cmp2);
  // The comparison
  return a > b ? -1 : (a < b ? 1 : 0);
  // A simpler, probably faster way:
  //return b - a;
}

/*quick sort example*/
void qsort_example(){
  float lt[6] = {35, 15, 80, 2, 40, 117};
  //   Number of items in the array
  int lt_length = sizeof(lt) / sizeof(lt[0]);
  //   qsort - last parameter is a function pointer to the sort function
  qsort(lt, lt_length, sizeof(lt[0]), sort_desc);
  //   lt is now sorted
  for (int i=0;i<lt_length;i++){
    Serial.print(String(lt[i])+" ");
  }
  Serial.print("\n");
}
