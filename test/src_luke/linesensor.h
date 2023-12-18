// this #ifndef stops this file
// from being included mored than
// once by the compiler.
#ifndef _LINESENSOR_H
#define _LINESENSOR_H


#define LED_PIN 13  // Pin to activate the orange LED
#define EMIT    11
#define LS_LEFT_PIN   A0
#define LS_CENTRE_PIN A2
#define LS_RIGHT_PIN  A3

// Define the max number of sensors to use.
# define NB_LS_PINS 3

// Measurements of max & min value of each sensors and their differences
//float L_max = 1520;
//float L_min = 650;
//float C_max = 1280;
//float C_min = 540;
//float R_max = 1260;
//float R_min = 550;
//
//float L_diff = L_max-L_min;
//float C_diff = C_max-C_min;
//float R_diff = R_max-R_min;

// Class to operate the linesensor(s).
class LineSensor_c {
  public:
    boolean led_state;  // Variable to "remember" the state
    // of the LED, and toggle it.
    int ls_pin[NB_LS_PINS] = { LS_LEFT_PIN, LS_CENTRE_PIN, LS_RIGHT_PIN };

    //Create an array to store the sensors readings
    float ls_reading[NB_LS_PINS];

    // Constructor, must exist.
    LineSensor_c() {

    }

    void initialise() {
      //Set the line sensors input pins mode as INPUT
      for (int which = 0; which < NB_LS_PINS; which++ ) {
        pinMode(ls_pin[which], INPUT);
      }
      //Set IR LEDS off to save some battery
      IREmitOff();
    }

    void IREmitOn() {
      //  Enable the IR LEDs by set it as OUTPUT(EMIT pin)
      pinMode(EMIT, OUTPUT);
      /*Write HIGH to EMIT pin to enable the IR LEDs for the line sensors*/
      /*Write LOW to EMIT pin to enable the IR LEDs for the bumper sensors*/
      digitalWrite(EMIT, HIGH);
    }
    void IREmitOff() {
      //  Enable the IR LEDs by set it as OUTPUT(EMIT pin)
      pinMode(EMIT, INPUT);
    }
    
    void parallelLineSensorRead() {
      IREmitOn();
      
      // We will use "which" to index through each
      // sensor.  Naming this variable "which" should
      // improve the readability of our code later.
      // You could use something else.
      int which;

      // This time, let's use a count to check how
      // many sensors have finished reading.  Let's
      // set it to NB_LS_PINS (3), and count down to
      // zero.  Therefore, count == 0 will mean all
      // sensors complete.
      int count = NB_LS_PINS;

      //We loop through all pins in array ls_pin[] to
      //charge the capacitors
      for ( which = 0; which < NB_LS_PINS; which++ ) {
        // Charge capacitor by setting input pin
        // temporarily to output and HIGH
        pinMode( ls_pin[ which ], OUTPUT );
        digitalWrite( ls_pin[ which ], HIGH );
      }
      // Tiny delay for capacitor to charge.
      delayMicroseconds(10);

      for ( which = 0; which < NB_LS_PINS; which++ ) {
        //  Turn output pin back to an input
        pinMode( ls_pin[ which ], INPUT );
      }

      // We still need to record the start time.
      unsigned long start_time;
      start_time = micros();

      // Multiple sensors, needs multiple places
      // to store the measurement result.
      // We follow the convention above to create
      // another array to store 3 values.
      unsigned long sensor_read[ NB_LS_PINS ]; // the values are stored in memory consecutively

      //Or use a list of booleans to check if a sensor has finished and been read
      //  boolean sensor_done[NB_LS_PINS];

      //Set end time to 0, which is our flag for when
      //the sensor(capacitor) has still not finished discharging(HIGH->LOW)
      for ( which = 0; which < NB_LS_PINS; which++ ) {
        sensor_read[which] = 0;
        //    sensor_done[which]=false;
      }

      //Use a boolean variable to act as a flag for when this operation is complete, start with false
      bool done = false;

      //Loop the read process until all 3 line sensors
      //have completed their read or timed out
      while (done == false) {
        // Whilst we are not done, we repeatedly attempt
        // to read the sensors.  We read the sensors in
        // a loop.  "which" will be 0,1,2 before moving on
        // with the code in this while loop.
        for ( which = 0; which < NB_LS_PINS; which++ ) {
          // We can instruct a digitalRead() using our
          // array variable to drop in the correct pin
          // number.  Here, "which" is incrementing
          // 0, 1, 2, looking up the pin value inside
          // ls_pin[].
          // Here, if digitalRead() returns LOW, it means
          // this sensor has completed.
          //       if (sensor_done[which] == false) {
          if (sensor_read[which] == 0) {
            if ( digitalRead( ls_pin[which] ) == LOW ) {

              //Store the time elapsed for this sensor
              sensor_read[which] = micros();

              //Sensor has finished, decrease count
              //When count is 0, all sensors are done
              count = count - 1;

              //              sensor_done[which] = true;
            }
          }
          //       Serial.println(String(which)+"sensor:"+String(sensor_read[which]));

          //We want to leave the while() loop once all
          //the sensors have reported a LOW signal (done)
          if (count == 0) {
            //Setting done to true will cause the while
            //loop to break
            done = true;
          } //if all sensors read
        }//for each sensor
      }//while not all sensors are read

      //Save the reading for each sensor
      for ( which = 0; which < NB_LS_PINS; which++ ) {
        unsigned long elapsed_time;
        elapsed_time = sensor_read[which] - start_time;
        ls_reading[which] = float(elapsed_time);
//        if (which==0){
//          ls_reading[which] = R_min+((ls_reading[which]-L_min)/L_diff)*R_diff;
//        }else if (which==1) {
//          ls_reading[which] = R_min+((ls_reading[which]-C_min)/C_diff)*R_diff;
//        }
      }

      IREmitOff();
    }//end of parallel read

    /*report readings*/
    void printSensorsReadings(){
      parallelLineSensorRead();
      for (int i = 0; i < NB_LS_PINS; i++) {
        if (i==0){
          Serial.print(String(ls_reading[i]));
        }else {
        Serial.print(','+String(ls_reading[i]));
        }
      }
      Serial.print('\n');
    }
    
};



#endif
