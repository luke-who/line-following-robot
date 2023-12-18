#ifndef _ENCODERS_H
#define _ENCODERS_H

#define ENCODER_0_A_PIN  7
#define ENCODER_0_B_PIN  23
#define ENCODER_1_A_PIN  26
//#define ENCODER_1_B_PIN Non-standard pin!

class Encoders_c {
  public:
    volatile long count_encdr_r;
    volatile byte state_encdr_r;                                        
    volatile long count_encdr_l;                                 
    volatile byte state_encdr_l; 
    volatile long last_count_l;
    volatile long last_count_r;  

  void init() {
    last_count_l = 0;
    last_count_r = 0;
    
    count_encdr_r = 0;
    pinMode( ENCODER_0_A_PIN, INPUT );
    pinMode( ENCODER_0_B_PIN, INPUT );
    state_encdr_r = 0;
    boolean e0_A = digitalRead( ENCODER_0_A_PIN );
    boolean e0_B = digitalRead( ENCODER_0_B_PIN );
    e0_A = e0_A ^ e0_B;
    state_encdr_r = state_encdr_r | ( e0_B << 1 );
    state_encdr_r = state_encdr_r | ( e0_A << 0 );
    EIMSK = EIMSK & ~(1<<INT6);
    EICRB |= ( 1 << ISC60 );
    EIFR |= ( 1 << INTF6 );
    EIMSK |= ( 1 << INT6 );

    
    count_encdr_l = 0;
    DDRE = DDRE & ~(1<<DDE6);
    PORTE = PORTE | (1 << PORTE2 );
    pinMode( ENCODER_1_A_PIN, INPUT );
    digitalWrite( ENCODER_1_A_PIN, HIGH ); 
    state_encdr_l = 0;
    boolean e1_B = PINE & (1<<PINE2);
    boolean e1_A = digitalRead( ENCODER_1_A_PIN );
    e1_A = e1_A ^ e1_B;
    state_encdr_l = state_encdr_l | ( e1_B << 1 );
    state_encdr_l = state_encdr_l | ( e1_A << 0 );
    PCICR = PCICR & ~( 1 << PCIE0 );
    PCMSK0 |= (1 << PCINT4);
    PCIFR |= (1 << PCIF0);
    PCICR |= (1 << PCIE0);
  }
};                                     

Encoders_c encoders;


// This ISR handles just Encoder 0
// ISR to read the Encoder0 Channel A and B pins
// and then look up based on  transition what kind of
// rotation must have occured.
ISR( INT6_vect ) {
  // We know that the ISR is only called when a pin changes.
  // We also know only 1 pin can change at a time.
  // The XOR(AB) signal change from "Channel A" triggers ISR.
 
  // First, Read in the new state of the encoder pins.
  // Standard pins, so standard read functions.
  boolean e0_B = digitalRead( ENCODER_0_B_PIN ); // normal B state
  boolean e0_A = digitalRead( ENCODER_0_A_PIN ); // XOR(AB)
 
  // Software XOR (^) logically infers
  // the true value of A given the state of B
  e0_A = e0_A ^ e0_B;

  // Shift our (new) current readings into bit positions
  // 2 and 3 in the state variable (current state)
  // State: (bit3)  (bit2)  (bit1)   (bit0)
  // State:  new B   new A   old B   old A
    encoders.state_encdr_r = encoders.state_encdr_r | ( e0_B  << 3 );
    encoders.state_encdr_r = encoders.state_encdr_r | ( e0_A  << 2 );

    // Handle which transition we have registered.
    // Complete this if statement as necessary.
    // Refer to the labsheet. 
    if( encoders.state_encdr_r == 0 ) {
      //no change
    } else if( encoders.state_encdr_r == 1 || encoders.state_encdr_r == 7 || encoders.state_encdr_r == 8 || encoders.state_encdr_r == 14) {
      encoders.count_encdr_r-=1; // forwards
    } else if( encoders.state_encdr_r == 2 || encoders.state_encdr_r == 4 || encoders.state_encdr_r == 11 || encoders.state_encdr_r == 13) {
      encoders.count_encdr_r+=1; // backwards
    } // Continue this if statement as necessary.

    // Shift the current readings (bits 3 and 2) down
    // into position 1 and 0 (to become prior readings)
    // This bumps bits 1 and 0 off to the right, "deleting"
    // them for the next ISR call. 
    encoders.state_encdr_r = encoders.state_encdr_r >> 2;

}


// This ISR handles just Encoder 1
// ISR to read the Encoder0 Channel A and B pins
// and then look up based on  transition what kind of
// rotation must have occured.
ISR( PCINT0_vect ) {
 
    // First, Read in the new state of the encoder pins.

    // Mask for a specific pin from the port.
    // Non-standard pin, so we access the register
    // directly.  
    // Reading just PINE would give us a number
    // composed of all 8 bits.  We want only bit 2.
    // B00000100 masks out all but bit 2
    // It is more portable to use the PINE2 keyword.
    boolean e1_B = PINE & (1<<PINE2);
    //boolean e1_B = PINE & B00000100;  // Does same as above.

    // Standard read from the other pin.
    boolean e1_A = digitalRead( ENCODER_1_A_PIN ); // 26 the same as A8


    e1_A = e1_A ^ e1_B;

    // Create a bitwise representation of our states
    // We do this by shifting the boolean value up by
    // the appropriate number of bits, as per our table
    // header:
    //
    // State :  (bit3)  (bit2)  (bit1)  (bit0)
    // State :  New A,  New B,  Old A,  Old B.
    encoders.state_encdr_l = encoders.state_encdr_l | ( e1_B  << 3 );
    encoders.state_encdr_l = encoders.state_encdr_l | ( e1_A  << 2 );


    // Handle which transition we have registered.
    // Complete this if statement as necessary.
    // Refer to the labsheet. 
    if( encoders.state_encdr_l == 0 ) {
      //no change
    } else if( encoders.state_encdr_l == 1 || encoders.state_encdr_l == 7 || encoders.state_encdr_l == 8 || encoders.state_encdr_l == 14) {
      encoders.count_encdr_l-=1;
    } else if( encoders.state_encdr_l == 2 || encoders.state_encdr_l == 4 || encoders.state_encdr_l == 11 || encoders.state_encdr_l == 13) {
      encoders.count_encdr_l+=1;
    } // Continue this if statement as necessary.

    // Shift the current readings (bits 3 and 2) down
    // into position 1 and 0 (to become prior readings)
    // This bumps bits 1 and 0 off to the right, "deleting"
    // them for the next ISR call. 
    encoders.state_encdr_l = encoders.state_encdr_l >> 2;
}


/*
   This setup routine enables interrupts for
   encoder1.  The interrupt is automatically
   triggered when one of the encoder pin changes.
   This is really convenient!  It means we don't
   have to check the encoder manually.
*/

#endif
