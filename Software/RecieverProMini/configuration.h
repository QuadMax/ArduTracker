//---------------------------------------------------------
//magnetic sensor configuration
//---------------------------------------------------------

  // Once you have your heading, you must then add your 
  // 'Declination Angle', which is the 'Error' of the 
  // magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // We need radians, so take your Magnatic declination 
  // and multiply it by (PI/180 )
  
  #define declinationAngle 0.02      //round at 3 digits


//---------------------------------------------------------
//voltage measurement
//---------------------------------------------------------

  // factor of your voltage divider, 
  // default 220kOhm and 100kOhm

  #define divider 3.2


//---------------------------------------------------------
//Baudrate to Computer
//---------------------------------------------------------

  #define BAUDRATE 115200
