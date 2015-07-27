#include "configuration.h"


unsigned long time,lasttime;
uint16_t timediff;

void setup() 
{  
  pinMode (LED, OUTPUT);  
  SerialOpen(BAUDRATE);
  get_heading();
  initPsk();
  digitalWrite(LED, 1);
  time = millis();
  lasttime = time;
}



float LonScale = 1.0;
float angleaz,anglel;
float sin_heading, cos_heading;

uint8_t c,i,d;

int16_t dlat;
int16_t dlon;
int16_t dalt;
int16_t heading;

int32_t  GPS_coord[2] = { 0, 0 };
uint8_t  GPS_numSat;
uint16_t GPS_altitude;
uint16_t GPS_speed;

int32_t  home[2] = { 0, 0 };
int16_t  home_alt = 0;
boolean  home_set = false;
extern uint16_t servo[2];
  

void loop() 
{

  
    if (receiveLTM())
    {
    digitalWrite(LED, 1);
      if (!home_set)
      {
        if (GPS_numSat >= 6)
        {
          home_set  = true;
          home[lat] = GPS_coord[lat];
          home[lon] = GPS_coord[lon];
          home_alt  = GPS_altitude;
          calc_lon_scale(); 
        }
      }
           
    // calculate angles
    calcangles();
      
    // output to pan / tilt servo
    angletoservo();
    writeServos();     

    lasttime = time;
  }
    
  if (time - lasttime >= 1050) 
  {
    digitalWrite(LED, 0);
    lasttime = time;
  }
  
}



