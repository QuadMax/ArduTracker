#if defined (RX)

/**************************************************************************************/
/***************                  Motor Pin order                  ********************/
/**************************************************************************************/

uint8_t PWM_PIN[2] = {9,10};   // arduino pins 9 + 10
uint16_t servo[2] = { 1500, 1500 }; // servo pulse in usec

/**************************************************************************************/
/***************   Writes the Servos values to the needed format   ********************/
/**************************************************************************************/
void writeServos() 
{
  OCR1A = servo[0]<<1; //  pin 9
  OCR1B = servo[1]<<1; //  pin 10
}


/**************************************************************************************/
/************        Initialize the PWM Timers and Registers         ******************/
/**************************************************************************************/

void initOutput() {

  /****************            mark all PWM pins as Output            ******************/
  for(uint8_t i=0;i<2;i++) pinMode(PWM_PIN[i],OUTPUT);
    
  /********  Specific PWM Timers & Registers for the atmega328P (Promini)   ************/  
  TCCR1A = (1<<WGM11) ; // 16 bit fast pwm
  TCCR1B = (1<<CS11) | (1<<WGM13) | (1<<WGM12); // clk/256
  ICR1 = 0x9FE0; // 20ms repetition rate
  
  TCCR1A |= _BV(COM1A1); // connect pin 9 to timer 1 channel A
  TCCR1A |= _BV(COM1B1); // connect pin 10 to timer 1 channel B
  
  writeServos();
}

#endif
