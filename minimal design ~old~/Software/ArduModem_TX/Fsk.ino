

// out pin 11
// in pin A3

#define FSK_BUFFER_SIZE 32
volatile uint8_t serialFSKHead,serialFSKTail;
volatile uint8_t serialFSKBuffer[FSK_BUFFER_SIZE];

uint8_t chksum;

#if defined (TX)

  // 1000hz gets -10% volume
  static const uint8_t sin_table_1000[32] =
  {
    127, 148, 169, 189, 206, 220, 230, 236, 
    239, 236, 230, 220, 206, 189, 169, 148, 
    127, 106,  85,  65,  48,  34,  24,  18, 
     15,  18,  24,  34,  48,  65,  85, 106, 
  };
  static const uint8_t sin_table_2000[32] = 
  {
    127, 174, 215, 242, 252, 242, 215, 174, 
    127, 80, 39, 12, 2, 12, 39, 80, 
    127, 174, 215, 242, 252, 242, 215, 174, 
    127, 80, 39, 12, 2, 12, 39, 80,
  };
#endif

void initFsk() 
{    
  #if defined (TX)
  // DA part
    pinMode(11,OUTPUT);    // 11 or 3 = output
    OCR2A = 128; // pin 11
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

// DA part
  
#if defined (TX)
  byte phase = 0;
  boolean is_1200 = true;
  boolean oddcycle = true;
  byte bitct = 0;
  byte data;

  ISR(TIMER2_OVF_vect) // this runs at 64khz
  {
    if (oddcycle) oddcycle = false;
    else
    {  
      oddcycle = true;    
      // this runs at 32 khz
      if (is_1200) OCR2A = sin_table_1000[phase];
      else         OCR2A = sin_table_2000[phase];
      phase++;
      if (phase >= 32) 
      {
        phase = 0;      
        // bitcounter async 0..7 data 
        // 8..9 stopbit
        // 10 idle, else startbit
        if (bitct <= 7) // databit
        {
          // get next bit 
          is_1200 = (data & (1<<(7-bitct)));
          bitct++;
        }
        else if (bitct <= 9)
        {
          //stopbit
          is_1200 = true;
          bitct++;
        }
        else if (bitct == 10)
        {
          //idle
          uint8_t t = serialFSKTail;
          if (serialFSKHead != t) {
            if (++t >= FSK_BUFFER_SIZE) t = 0;
            data = serialFSKBuffer[t];  // Transmit next byte in the ring
            bitct = 11;
            serialFSKTail = t;
          }
          is_1200 = true;
        }
        else
        {
          // startbit
          bitct = 0;
          is_1200 = false;
        }      
      }
    }
  }; 
  
  void writeFSK(uint8_t a) 
  {
    uint8_t t = serialFSKHead;
    if (++t >= FSK_BUFFER_SIZE) t = 0;
    serialFSKBuffer[t] = a;
    serialFSKHead = t;
    chksum ^= a;
  }
  void writeFSK16(int32_t a) 
  {
    writeFSK(a & 0xFF);
    writeFSK((a>>8) & 0xFF);
  }
  void writeFSK32(int32_t a) 
  {
    writeFSK(a & 0xFF);
    writeFSK((a>>8) & 0xFF);
    writeFSK((a>>16) & 0xFF);
    writeFSK((a>>24) & 0xFF);
  }

#endif
#if defined (RX)
  
  #define offs (128)
  
  int8_t fifo[4];
  int16_t sum = 0;
  int16_t rfifo[8];
  uint8_t rfifopt = 0;
  byte history = 0;
  byte bitct = 0;
  byte data;

  ISR(TIMER2_OVF_vect) // this runs at 8khz
  {
    int8_t currentSample;
    int16_t result;
    boolean is_space;
    
    //ad_byte = ADCH;
    //ad_byte_rdy++;
    //currentSample = ad_byte - offs;
    
    currentSample = ADCH - offs;  // store higher bytes ADC
    ADCSRA = 0xC7; // start AD conversion
    result = (currentSample * fifo[3]) >> 2;
    fifo[3] = fifo[2]; fifo[2] = fifo[1]; fifo[1] = fifo[0];
    fifo[0] = currentSample;
    
    sum -= rfifo[rfifopt];
    sum += result;
    rfifo[rfifopt] = result;
    rfifopt++;
    if(rfifopt >= 8) rfifopt = 0; 
    
    is_space = (sum <= 0);
    history <<= 1;
    if (is_space) history |= 0x01;

    if (bitct == 0)  
    {
      if (history == 0xF0) bitct = 1; // waiting for startbit 11110000
      else if ((history == 0x70) || (history == 0xB0) || (history == 0xD0)) bitct = 1;
      else if ((history == 0xF4) || (history == 0xF2) || (history == 0xF1)) bitct = 1;
    }
    else
    {     
      if      (bitct ==  9) data  = (is_space << 7);
      else if (bitct == 17) data |= (is_space << 6);
      else if (bitct == 25) data |= (is_space << 5);
      else if (bitct == 33) data |= (is_space << 4);
      else if (bitct == 41) data |= (is_space << 3);
      else if (bitct == 49) data |= (is_space << 2);
      else if (bitct == 57) data |= (is_space << 1);
      else if (bitct == 65) data |= (is_space << 0);
      
      //if (bitct == 1) digitalWrite(12, HIGH); //middle of bit check
      //if (bitct == 9) digitalWrite(12, LOW);
      
      bitct++;
      if (bitct >= 73)
      {
        uint8_t t = serialFSKHead;
        if (++t >= FSK_BUFFER_SIZE) t = 0;
        serialFSKBuffer[t] = data;
        serialFSKHead = t;
        bitct = 0;                
      }      
    }

    if (is_space) digitalWrite(12, HIGH); // marker low
    else digitalWrite(12, LOW); // marker high
  }; 

  uint8_t readFSK()
  {
    uint8_t t = serialFSKTail;
    if (serialFSKHead != t) 
    {
      uint8_t r;
      if (++t >= FSK_BUFFER_SIZE) t = 0;
      serialFSKTail = t;
      r = serialFSKBuffer[t];
      chksum ^= r;
      return r;
    }
    return 0;
  }

  uint16_t readFSK16() {
    uint16_t t = readFSK();
    t+= (uint16_t)readFSK()<<8;
    return t;
  }
  uint32_t readFSK32() {
    uint32_t t = readFSK16();
    t+= (uint32_t)readFSK16()<<16;
    return t;
  }

#endif

uint8_t FskAvailable() 
{
  return ((uint8_t)(serialFSKHead - serialFSKTail))%FSK_BUFFER_SIZE;
}


