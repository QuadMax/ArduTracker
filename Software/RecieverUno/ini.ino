void get_heading(){
  if (SerialAvailable() > 1 )
  {
    byte b1 = SerialRead();
    byte b2 = SerialRead();
    int heading = b1 * 256 + b2;
  }
  else
  {
    if (compass)
    {
      get_heading();
    }
    else
    {
      heading = 0;
    }
  }  
}



void calc_lon_scale() 
{
  float lati = (float)home[lat];
  float rads = (abs((float)lati) / 10000000.0) * 0.0174532925;
  LonScale = cos(rads) * 0.01113195f;

  sin_heading = sin(heading);
  cos_heading = cos(heading);
}


void calcangles() 
{
  dlat = (int)(GPS_coord[lat] - home[lat]) * 0.01113195f;
  dlon = (int)(GPS_coord[lon] - home[lon]) * LonScale;
  dalt = (int)(GPS_altitude -home_alt);

  //it is gonna really complex now
 
  //calculating pseudo coordinates:
  int16_t pseudoposx = (int)(-sin_heading*(dlat * cos_heading - dlon * sin_heading));
  int16_t pseudoposy = (int)(cos_heading*(dlat*cos_heading - dlon * sin_heading));
 
  // z value is the same as the target ones, so i leaf it at dalt 
 
  // calculating distance from pseudopoint to target and to ground station
  int16_t pseudobottomtobase = (int)(sqrt(pseudoposx*pseudoposx+pseudoposy*pseudoposy));
  int16_t pseudotobase = (int)(sqrt(pseudobottomtobase*pseudobottomtobase+dalt*dalt));
  int16_t pseudototarget = (int)(sqrt((pseudoposx-dlon)*(pseudoposx-dlon)+(pseudoposy-dlat)*(pseudoposy-dlat)));

   // calculating angels for servos now

   angleaz = 90 - atan(dalt/(pseudobottomtobase))* 57.296;    // tower servo
   anglel = atan(pseudototarget/pseudotobase) * 57.296;       // arm servo 
 
   // checking if the servos turn the right way
  GPS_heading = (int16_t)(atan2(dlat,dlon) * 57.29578f + 270);
  if(GPS_heading > 360) GPS_heading -=360;

  diff_heading = GPS_heading - heading;

  if( diff_heading > 90 && diff_heading < 270 ) angleaz = angleaz *(-1);
  if( diff_heading > 180 ) anglel = anglel *(-1);

  if (tower_inv) angleaz = angleaz * (-1);
  if (arm_inv) anglel = anglel * (-1);
}


void angletoservo()
{
  servo[0] = center + (int)(angleaz * az_fact);
  servo[1] = center + (int)(anglel * el_fact);
}

void invert_servo_jumper()
{
  if(tower == HIGH) tower_inv = true;
  if(arm ==HIGH) arm_inv = true;
}

