#include "configuration.h"


unsigned long time,lasttime;
uint16_t timediff;



float LonScale = 1.0;
float angleaz,anglel;
float sin_heading, cos_heading;

uint8_t c,i,d;

int16_t dlat;
int16_t dlon;
int16_t dalt;
int16_t heading;
int16_t GPS_heading;
int16_t diff_heading;

int32_t  GPS_coord[2] = { 0, 0 };
uint8_t  GPS_numSat;
uint16_t GPS_altitude;
uint16_t GPS_speed;

int32_t  home[2] = { 0, 0 };
int16_t  home_alt = 0;
boolean  home_set = false;
boolean tower_inv = false;
boolean arm_inv = false;
extern uint16_t servo[2];

void setup() 
{  
  pinMode(tower, INPUT);
  pinMode(arm, INPUT);  
  SerialOpen(BAUDRATE);
  get_heading();
  initPsk();
  time = millis();
  lasttime = time;

  invert_servo_jumper();
}


  

void loop() 
{

  
    if (receiveLTM())
    {
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
    lasttime = time;
  }
  
}



