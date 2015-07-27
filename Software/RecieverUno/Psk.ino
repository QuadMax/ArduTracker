uint8_t chksum;

// out pin 11
// in pin A3

#define PSK_BUFFER_SIZE 64
volatile uint8_t inBufPt,outBufPt;
volatile uint8_t serialPSKBuffer[PSK_BUFFER_SIZE];

// 32khz sampling , 64khz / 2
// 128 samples per cycle 
// 32000 / 128 = 250 baud
// 4 cycles per bit, 250 * 4 = 1000hz

#if defined (RX)
  #define offs (128)
  #define mydelay 32  //bittime = 4ms -> 250 baud
#endif

void initPsk() 
{     
  #if defined (RX)
  // AD part
    OCR2A = 128; // pin 11
    TCCR2B =  (1<<CS21);   // run at 8khz
    TCCR2A =  0;           // normal mode
    TIMSK2 |= (1<<TOIE2);  // enable tim2 interrupt
    
    pinMode(12, OUTPUT);   // pin = output  
    DIDR0 = 0x3F;          // digital inputs disabled
    #if defined (INTVREF)
      ADMUX = 0xE3;          // measuring on ADC3, use 1.1V reference, left adjust
    #else
      ADMUX = 0x63;          // measuring on ADC3, use VCC reference, left adjust
    #endif
    ADCSRA = 0x87;         // AD-converter on, interrupt disabled, prescaler = 128
    ADCSRB = 0x00;         // AD channels MUX on
  #endif

  sei();                   // set interrupt flag
}

#if defined (RX)

  int8_t fifo[mydelay];
  uint8_t fifopt = 0;
  
  #define LOWRES
  
  int16_t sum = 0;
  #if defined (LOWRES)
    int8_t rfifo[mydelay]; // 8 bit variant, low dynamic range, 8us
  #else
    int16_t rfifo[mydelay]; // 16 bit variant, 9-12 us all 125us
  #endif
  
  byte history = 0;
  byte bitct = 0;
  byte data;
  byte decimationct = 0;

  ISR(TIMER2_OVF_vect) // this runs at 8khz
  {
    uint8_t ad_byte;
    int8_t currentSample;
    int8_t oldestSample;
    int16_t result;
    boolean is_space;
    
    ad_byte = ADCH;   
    
    #if defined(TESTRXLEVEL)
      SerialWrite(ad_byte);
      ADCH = 127;
      ADCSRA = 0xC7; // start AD conversion
    #else
    
    currentSample = ad_byte - offs;
    ADCSRA = 0xC7; // start AD conversion
    oldestSample = fifo[fifopt];
    fifo[fifopt] = currentSample;
    // incrase the thing later
    
    #if defined (LOWRES)
      // may be 127 * 127 = +-16300>>7 = +-64 
      result = (currentSample * oldestSample) >> 8;
    
      // summed over 32 samples = 64 * 32 = 2016
      sum -= rfifo[fifopt];
      sum += result;
      rfifo[fifopt] = result;
    #else
      // may be 127 * 127 = +-16300>>3 = +-2000 
      result = (currentSample * oldestSample) >> 3; 
    
      // summed over 32 samples = 2000 * 32 = 64000 
      sum -= rfifo[fifopt];
      sum += result;
      rfifo[fifopt] = result;
    #endif
    
    fifopt = (fifopt + 1)%mydelay;
    
    is_space = (sum <= 0);
    
    // fix me. 32 bits / symbol to 4 bits/symbol
    decimationct++; //counts 1,2,3,4 from here on
    //bitphase := bitphase + (early - late) / (early + late);
    if (decimationct == 4)
    {
      decimationct = 0;
            
      history <<= 1;  // history should be 64 bits long ....
      if (is_space) history |= 0x01;
    
      if (bitct == 0)  
      {
        if (history == 0xF0) bitct = 1; // waiting for startbit 11110000
        else if ((history == 0x70) || (history == 0xB0) || (history == 0xD0)) bitct = 1;
        else if ((history == 0xF4) || (history == 0xF2) || (history == 0xF1)) bitct = 1;
      }
      else
      {    
        // bitct advances every half ms. 250 baud = 4ms / bit + 1 start + 1 stop 
        if      (bitct ==  8) data  = (is_space << 0);
        else if (bitct == 16) data |= (is_space << 1);
        else if (bitct == 24) data |= (is_space << 2);
        else if (bitct == 32) data |= (is_space << 3);
        else if (bitct == 40) data |= (is_space << 4);
        else if (bitct == 48) data |= (is_space << 5);
        else if (bitct == 56) data |= (is_space << 6);
        else if (bitct == 64) data |= (is_space << 7);
      
        //if (bitct%8 == 0) digitalWrite(12, HIGH); //middle of bit check
        //if (bitct%8 == 1) digitalWrite(12, LOW);
      
        bitct++;
        if (bitct >= 72)
        {
          uint8_t t = inBufPt;
          if (++t >= PSK_BUFFER_SIZE) t = 0;
          serialPSKBuffer[t] = data;
          inBufPt = t;
          bitct = 0;  
        }      
      }
    
      if (is_space) digitalWrite(12, HIGH); // marker low
      else digitalWrite(12, LOW); // marker high
    }
    #endif //TESTRXLEVEL    
  }; 

  uint8_t readPSK()
  {
    uint8_t t = outBufPt;
    if (inBufPt != t) 
    {
      uint8_t r;
      if (++t >= PSK_BUFFER_SIZE) t = 0;
      outBufPt = t;
      r = serialPSKBuffer[t];
      chksum ^= r;
      return r;
    }
    return 0;
  }

  uint16_t readPSK16() {
    uint16_t t = readPSK();
    t+= (uint16_t)readPSK()<<8;
    return t;
  }
  uint32_t readPSK32() {
    uint32_t t = readPSK16();
    t+= (uint32_t)readPSK16()<<16;
    return t;
  }

  uint8_t PskAvailable() 
  {
    return ((uint8_t)(inBufPt - outBufPt))%PSK_BUFFER_SIZE;
  }
#endif


