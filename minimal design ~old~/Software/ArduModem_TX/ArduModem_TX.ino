
// AFSK out is pin 11

#define TX  // TX or RX firmware
#define LED 13
#define BAUDRATE 57600

unsigned long time,lasttime;

void setup() 
{
  pinMode (LED, OUTPUT);  
  SerialOpen(BAUDRATE); // GPS baudrate
  initFsk();
  digitalWrite(LED, 1);
  time = millis();
  lasttime = time;
}

#define lat 0
#define lon 1

int32_t  GPS_coord[2] = { 0, 0 };
uint8_t  GPS_numSat;
uint16_t GPS_altitude;
uint16_t GPS_speed;
  
extern uint8_t  GPS_Present;

uint8_t missedframes = 0;
uint8_t goodframes = 0;

void loop() 
{
  uint8_t c;
  boolean gotframe;
  
  // read gps data
  c = SerialAvailable();
  while (c--) 
  {
    gotframe = GPS_NMEA_newFrame(SerialRead());
    if (gotframe) digitalWrite(LED, 1);
  }

  time = millis(); 
  if (time - lasttime >= 500) // 1000 baud = 100 char /sec
  {
    // send data over modem
    lasttime = time;
    sendLTM();
    if (GPS_numSat < 5) digitalWrite(LED, 0);
    GPS_numSat = 0; 
  }        
}



