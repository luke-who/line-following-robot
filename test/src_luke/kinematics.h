// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _KINEMATICS_H
#define _KINEMATICS_H

// Class to track robot position.
class Kinematics_c {
  public:
    float dis_per_count = 2.f*PI*16.f/358.3f;

    long phi_l;
    long phi_r;
    
    //displacement in the local co-ordinate frame
    float X_R;
    float Y_R;
    float theta_R;
    
    //displacement in the global co-ordinate frame
    float X_I;
    float Y_I;
    float theta_I;
    // Constructor, must exist.
    Kinematics_c() {

    } 
    
    void initialise(){
      phi_l = 0;
      phi_r = 0;
      
      //initialise displacement in the local co-ordinate frame
      X_R = 0;
      Y_R = 0;
      theta_R = 0;
      
      //initialse displacement in the global co-ordinate frame
      X_I = 0;
      Y_I = 0;
      theta_I = 0;     
    }
    // Use this function to update
    // your kinematics
    void update() {
      phi_l*=dis_per_count;
      phi_r*=dis_per_count;
      
      X_R = -(phi_l+phi_r)/2.f;
      theta_R = (phi_l-phi_r)/(2.f*43.f);
        
      X_I += X_R*cos(theta_I);
      Y_I += X_R*sin(theta_I);
      theta_I += theta_R;

//      Serial.println("phi_l:"+String(phi_l)+",phi_r:"+String(phi_r));
//      Serial.println("X_R:"+String(X_R)+",theta_R:"+String(theta_R));
      Serial.println("X_I:"+String(X_I)+",Y_I:"+String(Y_I)+",theta_I:"+String(theta_I));
    }

};



#endif
