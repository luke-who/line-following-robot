// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _PID_H
#define _PID_H

// Class to contain generic PID algorithm.
class PID_c {
  public:
    // Gains
    float Kp;
    float Ki;
    float Kd;
    
    // PID terms
    float p_term;
    float i_term;
    float d_term;

    // Error from the last timestep
    float last_error;
    
    // Variable to store the sum of all the integral values across all timesteps
    float i_sum;
    
    // Sum of all PID terms
    float feedback;

    // Store the elapsed time
    unsigned long ms_last_ts;
    
    // Constructor, must exist.
    PID_c() {

    } 

    void initialise(float kp,float ki,float kd){
      Kp = kp;
      Ki = ki;
      Kd = kd;

      p_term = 0;
      i_term = 0;
      d_term = 0;

      i_sum = 0;
      
      last_error = 0.f;
      feedback = 0.f;
      
      ms_last_ts = millis();
    }

    float update(float demand,float measurement){
      float error;
      unsigned long delta_t;
      float error_diff;

      // Calculate elapsed time and type cast it to float
      delta_t = float(millis()-ms_last_ts);

      // Update ms_last_ts for the next timestep
      ms_last_ts = millis();

      // Catch the division by 0 (dt) error resulting from PID.update() is called faster than 1ms
      if (delta_t==0.f) {
        return feedback;
      }

      // Calculate error signal
      error = demand - measurement;

      /* P term */
      p_term = Kp*error;

      //discrete integration 
      i_sum += error * delta_t;
      /* I term */
      i_term = Ki*i_sum;
  
      /* D term */
      // Note soemtimes it needs to be last_error - error depends on the 
      // error sign in relation to the system
      error_diff = (error - last_error) / delta_t;
      d_term = Kd*error_diff;
      
      last_error = error;

      feedback = p_term + i_term + d_term;
      
      return feedback;
    }
    
    void reset(){

      p_term = 0;
      i_term = 0;
      d_term = 0;

      i_sum = 0;
      
      last_error = 0.f;
      feedback = 0.f;
      
      ms_last_ts = millis();      
    }

};



#endif
