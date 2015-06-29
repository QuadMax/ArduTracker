
#define RX  // TX or RX firmware
//#define INTVREF  // use internal 1.1V ref for AD
#define LED 13
#define BAUDRATE 115200
#define FSK_DEBUG

// PWM out is pin 9 and 10
// AFSK out is pin 11
// AFSK in is pin A3

void setup() 
{  
  pinMode (LED, OUTPUT);  
  SerialOpen(BAUDRATE);
  initFsk();
  digitalWrite(LED, 1);
}

volatile uint8_t ad_byte;
volatile uint8_t ad_byte_rdy;

void loop() 
{
  if (ad_byte_rdy)
  {
    ad_byte_rdy = 0;
    SerialWrite(ad_byte);
  } 
}



