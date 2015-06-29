
#define RX  // TX or RX firmware
//#define INTVREF  // use internal 1.1V ref for AD
#define LED 13
#define BAUDRATE 115200

// PWM out is pin 9 and 10
// AFSK out is pin 11
// AFSK in is pin A3

unsigned long time,lasttime;

void setup() 
{  
  pinMode (LED, OUTPUT);  
  SerialOpen(BAUDRATE);
  initOutput();
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
  
int32_t  GPS_home[2] = { 0, 0 };
int32_t  home_alt = 0;
boolean  home_set = false;
extern uint16_t servo[2];

float angaz,angel;
  
float new_lat = 0.0;
float new_lon = 0.0;
float new_alt = 0.0;
float old_lat = 0.0;
float old_lon = 0.0;
float old_alt = 0.0;
  
float act_lat = 0.0;
float act_lon = 0.0;
float act_alt = 0.0;
  
float LonScale = 1.0;
  
void calc_lon_scale(int32_t lati) 
{
  float rads = (abs((float)lati) / 10000000.0) * 0.0174532925;
  LonScale = cos(rads);
}

void updatepos()
{
  old_lat = new_lat;
  old_lon = new_lon;
  old_alt = new_alt;
    
  new_lat = (float)(GPS_coord[lat] - GPS_home[lat]);
  new_lon = (float)(GPS_coord[lon] - GPS_home[lon]) * LonScale;
  new_alt = (float)(GPS_altitude - home_alt);
}
  
float dist;
    
void calcangles()
{
  if(new_alt < 0.5) new_alt = 0.5;
  if(new_lat <= 0) new_lat = 0.1;
  
  float angleaz,angleel;
  dist = sqrt(act_lat*act_lat + act_alt*act_alt);
  angleel = atan(act_lat/act_alt);  
  angleaz = atan(act_lon/dist);      
  
  angaz = angleaz * 57.296;      
  angel = angleel * 57.296;
}
  
#define az_fact 5.5
#define el_fact 5.5

void angletoservo()
{
  servo[0] = 1500 + (int)(angaz * az_fact);                
  servo[1] = 1500 + (int)(angel * el_fact);
}
  
uint8_t missedframes = 0;
uint8_t goodframes = 0;

//volatile uint8_t ad_byte;
//volatile uint8_t ad_byte_rdy;

void loop() 
{
  uint8_t c,i,d;
  
  time = millis();
    
  /*
  if (ad_byte_rdy)
  {
    ad_byte_rdy = 0;
    SerialWrite(ad_byte);
  } 
  */
    
  if (receiveLTM())
  {
    digitalWrite(LED, 1);
    if (!home_set)
    {
      if (GPS_numSat >= 6)
      {
        home_set = true;
        goodframes = 0;
        missedframes = 0;
        GPS_home[lat] = GPS_coord[lat];
        GPS_home[lon] = GPS_coord[lon];
        home_alt      = GPS_altitude;
        calc_lon_scale(GPS_coord[lat]); 
      }
    }
      
    // do gps calculations
    updatepos();
      
    act_lat = new_lat;
    act_lon = new_lon;
    act_alt = new_alt;
           
    // calculate angles
    calcangles();
      
    // output to pan / tilt servo
    angletoservo();
    writeServos();     

    lasttime = time;
      
    goodframes++;
    serialize8(goodframes);
    serialize8(missedframes);
    SerialWrite(13);
    //SerialWrite((int)(angaz*0.1));
  }
    
  if (time - lasttime >= 1050) 
  {
    digitalWrite(LED, 0);
    lasttime = time;
    missedframes++;
    serialize8(goodframes);
    serialize8(missedframes);
    SerialWrite(13);
  }
}



