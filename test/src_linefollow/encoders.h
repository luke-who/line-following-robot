#define ENCODER_RIGHT_A  7
#define ENCODER_RIGHT_B  23
#define ENCODER_LEFT_A  26

class Encoders_c {
  public:
    volatile boolean right_a, right_b, left_a, left_b;
    volatile long right_count, left_count = 0;
    volatile byte right_state, left_state = 0;

    Encoders_c() {
    }
    void init() {
      pinMode(ENCODER_RIGHT_A, INPUT);
      pinMode(ENCODER_RIGHT_B, INPUT);
      pinMode(ENCODER_LEFT_A, INPUT);
      DDRE &= ~(1 << DDE6);
      PORTE |= (1 << PORTE2);

      EIMSK &= ~(1 << INT6);
      EICRB |= (1 << ISC60);
      EIFR |= (1 << INTF6);
      EIMSK |= (1 << INT6);

      PCICR &= ~(1 << PCIE0);
      PCMSK0 |= (1 << PCINT4);
      PCIFR |= (1 << PCIF0);
      PCICR |= (1 << PCIE0);
    }
    long left_velocity(){
      long velocity = left_count;
      left_count = 0;
      return velocity;
    }
    long right_velocity(){
      long velocity = right_count;
      right_count = 0;
      return velocity;
    }
};

Encoders_c encoders;

ISR(INT6_vect) {
  encoders.right_a = digitalRead(ENCODER_RIGHT_A);
  encoders.right_b = digitalRead(ENCODER_RIGHT_B);
  encoders.right_a = encoders.right_a ^ encoders.right_b;
  encoders.right_state = encoders.right_state << 2;
  encoders.right_state |= encoders.right_a << 1;
  encoders.right_state |= encoders.right_b;
  if (((encoders.right_state >> 3) & 1) ^ (encoders.right_state & 1)) {
    encoders.right_count -= 1;
  } else {
    encoders.right_count += 1;
  }
//  Serial.println(encoders.right_count);
}

ISR(PCINT0_vect) {
  encoders.left_a = digitalRead(ENCODER_LEFT_A);
  encoders.left_b = PINE & (1 << PINE2);
  encoders.left_a = encoders.left_a ^ encoders.left_b;
  encoders.left_state = encoders.left_state << 2;
  encoders.left_state |= encoders.left_a << 1;
  encoders.left_state |= encoders.left_b;
  if (((encoders.left_state >> 3) & 1) ^ (encoders.left_state & 1)) {
    encoders.left_count -= 1;
  } else {
    encoders.left_count += 1;
  }
//  Serial.println(encoders.left_count);
}
