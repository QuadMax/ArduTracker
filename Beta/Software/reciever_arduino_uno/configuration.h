//---------------------------------------------------------
//magnetic sensor configuration
//---------------------------------------------------------

  // Once you have your heading, you must then add your 
  //'Declination Angle', which is the 'Error' of the 
  //magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // We need radians, so take your Magnatic declination 
  //and multiply it by (PI/180 )
  #define declinationAngle 0.02      //round at 3 digits
 
  
//---------------------------------------------------------
//internal 1.1V ref for AD
//---------------------------------------------------------

  #define INTVREF  // use internal 1.1V ref for AD


//---------------------------------------------------------
//Baudrate to Computer
//---------------------------------------------------------

  #define BAUDRATE 115200


//---------------------------------------------------------
//Debugging mode 
//---------------------------------------------------------

  //#define TESTRXLEVEL  //only turn on for debugging


//---------------------------------------------------------
//Tower servo
//---------------------------------------------------------

  #define standard    //180 degree 1000us to 2000us
  //#define extended    //270 degree 500us to 2500us


//#########################################################
// Do not touch anything under this line
//#########################################################


  #define RX  // TX or RX firmware
  #define LED 13
  #define lat 0
  #define lon 1

  
  #if defined (standard)
    #define az_fact 2.777
    #define el_fact 2.777
    #define center 1500
  #endif

  #if defined (extended)
    #define az_fact 2.777
    #define el_fact 5.555
    #define center 1500
  #endif
