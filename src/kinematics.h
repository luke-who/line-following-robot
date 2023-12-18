// this #ifndef stops this file
// from being included mored than
// once by the compiler. 

#ifndef _KINEMATICS_H
#define _KINEMATICS_H

#define KINEMATIC_TIMESTEP 2

// Class to track robot position.
class Kinematics_c  {
  public:
    Encoders_c *encoders_in;
    float dis_per_count = 2.f*PI*16.f/358.3f;

    float phi_l;
    float phi_r;
    unsigned long last_update;
    
    //displacement in the local co-ordinate frame
    float X_R;
    float Y_R;
    float theta_R;
    
    //displacement in the global co-ordinate frame
    float X_I;
    float Y_I;
    float theta_I;
    // Constructor, must exist.
    Kinematics_c(Encoders_c *encoders_i) {
      encoders_in = encoders_i;
    } 
    
    void initialise(){
      phi_l = 0;
      phi_r = 0;
      
      //initialise displacement in the local co-ordinate frame
      X_R = 0;
      theta_R = 0;
      
      //initialse displacement in the global co-ordinate frame
      X_I = 0;
      Y_I = 0;
      theta_I = 0;     
      last_update = millis();
    }
    
    // Use this function to update
    // your kinematics
    void update() {
      if (millis() - last_update >= KINEMATIC_TIMESTEP){
        phi_l=float(encoders_in->last_count_l-encoders_in->count_encdr_l);
        phi_r=float(encoders_in->last_count_r-encoders_in->count_encdr_r);

        encoders_in->last_count_l=encoders_in->count_encdr_l;
        encoders_in->last_count_r=encoders_in->count_encdr_r;
  
        phi_l*=dis_per_count;
        phi_r*=dis_per_count;
        
        X_R = -(phi_l+phi_r)/2.f;
        theta_R = (phi_l-phi_r)/(2.f*43.f);
          
        X_I += X_R*cos(theta_I);
        Y_I += X_R*sin(theta_I);
        theta_I += theta_R;
 

//        Serial.println("X_I:"+String(X_I)+",Y_I:"+String(Y_I)+",theta_I:"+String(theta_I));
        last_update = millis();
      }
    }
};



#endif
