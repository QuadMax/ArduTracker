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

#if defined (TX)
  #define mydelay 128
  #define SINTABLEN 32
  static const uint8_t sin_table_same[SINTABLEN] =
  {
    151, 174, 195, 214, 230, 241, 248, 251, 248, 241, 230, 214, 195, 174, 151, 127, 
    103, 80, 59, 40, 24, 13, 6, 3, 6, 13, 24, 40, 59, 80, 103, 127, 
  };
  static const uint8_t sin_table_change[mydelay] = 
  {
    151, 174, 195, 214, 229, 240, 246, 248, 245, 238, 226, 210, 192, 171, 149, 127, 
    105, 85, 66, 50, 38, 29, 25, 24, 28, 35, 46, 60, 75, 92, 110, 127, 
    143, 158, 171, 182, 190, 195, 197, 195, 192, 185, 177, 168, 157, 147, 136, 127, 
    119, 112, 106, 102, 100, 100, 101, 103, 107, 111, 115, 119, 122, 125, 127, 127, 
    127, 125, 122, 119, 115, 111, 107, 103, 101, 100, 100, 102, 106, 112, 119, 127, 
    136, 147, 157, 168, 177, 185, 192, 195, 197, 195, 190, 182, 171, 158, 143, 127, 
    110, 92, 75, 60, 46, 35, 28, 24, 25, 29, 38, 50, 66, 85, 105, 127, 
    149, 171, 192, 210, 226, 238, 245, 248, 246, 240, 229, 214, 195, 174, 151, 127,
  };
#endif

#if defined (RX)
  #define offs (128)
  #define mydelay 32  //bittime = 4ms -> 250 baud
#endif

void initPsk() 
{    
  #if defined (TX)
    pinMode(11,OUTPUT);    // 11 or 3 = output
    OCR2A = 128;  
    TCCR2B =  (1<<CS20);   //run at 64khz
    TCCR2A = _BV(COM2A1) | _BV(WGM20)| _BV(WGM21); // connect pin 11 to timer 2 channel A
    TIMSK2 |= (1<<TOIE2);  // enable tim2 interrupt
  #endif
  
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

#if defined (TX)
  byte phase = 0;
  boolean is_change = true;
  boolean is_invert = false;
  byte oddcycle = 0;
  byte bitct = 0;
  byte data;
  
  ISR(TIMER2_OVF_vect) // this runs at 64khz
  {
    if (oddcycle < 1) oddcycle++;
    else
    {  
      oddcycle = 0;    
      // this runs at 32 khz
      if (is_change) 
      {
        byte sinval = sin_table_change[phase];
        if (is_invert) OCR2A = sinval ^ 0xFF;
        else           OCR2A = sinval; 
      }  
      else 
      {
        byte sinval = sin_table_same[phase%SINTABLEN];
        if (is_invert) OCR2A = sinval ^ 0xFF;
        else           OCR2A = sinval; 
      }
      phase++;
      if (phase >= mydelay) 
      {
        if (is_change) is_invert = !is_invert;
        phase = 0; 
      
        // bitcounter 0..7 data / 8 stopbit / 9 idle, else startbit
        if (bitct <= 7) // databit
        {
          // get next bit 
          is_change = (data & (1<<bitct));
          bitct++;
        }
        else if (bitct == 8)
        {
          //stopbit 1000 hz
          is_change = true;
          bitct++;
        }
        else if (bitct == 9)
        {
          if ((uint8_t)(inBufPt - outBufPt)%PSK_BUFFER_SIZE > 0) 
          {
            outBufPt++; 
            if (outBufPt >= PSK_BUFFER_SIZE) outBufPt = 0;
            data = serialPSKBuffer[outBufPt];  
            // startbit no_change
            bitct = 0;
            is_change = false;
          }
          else is_change = true;        
        }
      }
    }
  }
   
  void writePSK(uint8_t a) 
  {
    uint8_t t = inBufPt;
    if (++t >= PSK_BUFFER_SIZE) t = 0;
    serialPSKBuffer[t] = a;
    inBufPt = t;
    chksum ^= a;
  }
  void writePSK16(int32_t a) 
  {
    writePSK(a & 0xFF);
    writePSK((a>>8) & 0xFF);
  }
  void writePSK32(int32_t a) 
  {
    writePSK(a & 0xFF);
    writePSK((a>>8) & 0xFF);
    writePSK((a>>16) & 0xFF);
    writePSK((a>>24) & 0xFF);
  }
#endif

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


