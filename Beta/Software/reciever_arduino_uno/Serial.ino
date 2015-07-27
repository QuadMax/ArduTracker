
#define RX_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 64

static volatile uint8_t serialHeadRX,serialTailRX;
static uint8_t serialBufferRX[RX_BUFFER_SIZE];
static volatile uint8_t serialHeadTX,serialTailTX;
static uint8_t serialBufferTX[TX_BUFFER_SIZE];

void serialize8(uint8_t a) 
{
  uint8_t t = serialHeadTX;
  if (++t >= TX_BUFFER_SIZE) t = 0;
  serialBufferTX[t] = a;
  serialHeadTX = t;
}

ISR(USART_UDRE_vect) 
{  // Serial 0 on a PROMINI
  uint8_t t = serialTailTX;
  if (serialHeadTX != t) {
    if (++t >= TX_BUFFER_SIZE) t = 0;
    UDR0 = serialBufferTX[t];  // Transmit next byte in the ring
    serialTailTX = t;
  }
  if (t == serialHeadTX) UCSR0B &= ~(1<<UDRIE0); // Check if all data is transmitted . if yes disable transmitter UDRE interrupt
}

void UartSendData() 
{
  UCSR0B |= (1<<UDRIE0);
}

static void inline SerialOpen(uint32_t baud) 
{
  uint8_t h = ((F_CPU  / 4 / baud -1) / 2) >> 8;
  uint8_t l = ((F_CPU  / 4 / baud -1) / 2);
  UCSR0A  = (1<<U2X0); UBRR0H = h; UBRR0L = l; UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
}

static void inline SerialEnd(uint8_t port) 
{
  UCSR0B &= ~((1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<UDRIE0));
}

static void inline store_uart_in_buf(uint8_t data) 
{

  uint8_t h = serialHeadRX;
  if (++h >= RX_BUFFER_SIZE) h = 0;
  if (h == serialTailRX) return; // we did not bite our own tail?
  serialBufferRX[serialHeadRX] = data;  
  serialHeadRX = h;
}

ISR(USART_RX_vect)  { store_uart_in_buf(UDR0); }

uint8_t SerialRead() 
{
  uint8_t t = serialTailRX;
  uint8_t c = serialBufferRX[t];
  if (serialHeadRX != t) {
    if (++t >= RX_BUFFER_SIZE) t = 0;
    serialTailRX = t;
  }
  return c;
}

uint8_t SerialAvailable() 
{
  return ((uint8_t)(serialHeadRX - serialTailRX))%RX_BUFFER_SIZE;
}

void SerialWrite(uint8_t c)
{
  serialize8(c);UartSendData();
}

//------------------------------------------------

char digit10000(uint16_t v) {return '0' + v / 10000;}
char digit1000(uint16_t v) {return '0' + v / 1000 - (v/10000) * 10;}
char digit100(uint16_t v) {return '0' + v / 100 - (v/1000) * 10;}
char digit10(uint16_t v) {return '0' + v / 10 - (v/100) * 10;}
char digit1(uint16_t v) {return '0' + v - (v/10) * 10;}

void printChar(const char *s) 
{
  while (*s) {serialize8(*s++);}
}

void print_uint8(uint8_t unit) 
{
  char line[3];
  line[0] = digit100(unit);
  line[1] = digit10(unit);
  line[2] = digit1(unit);
  printChar(line);
}
void print_int8(int8_t v) 
{
  char line[4];
  uint8_t unit;
  if (v < 0 ) {
    unit = -v;
    line[0] = '-';
  } else {
    unit = v;
    line[0] = ' ';
  }
  line[1] = digit100(unit);
  line[2] = digit10(unit);
  line[3] = digit1(unit);
  printChar(line);
}
void print_uint16(uint16_t unit) 
{
  char line[6];
  line[0] = digit10000(unit);
  line[1] = digit1000(unit);
  line[2] = digit100(unit);
  line[3] = digit10(unit);
  line[4] = digit1(unit);
  printChar(line);
}
void print_int16(int16_t v) {
  uint16_t unit;
  char line[7];
  if (v < 0 ) {
    unit = -v;
    line[0] = '-';
  } else {
    unit = v;
    line[0] = ' ';
  }
  line[1] = digit10000(unit);
  line[2] = digit1000(unit);
  line[3] = digit100(unit);
  line[4] = digit10(unit);
  line[5] = digit1(unit);
  printChar(line);
}

