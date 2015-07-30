//---------------------------------------------------------
//magnetic sensor configuration
//---------------------------------------------------------

  #define compass  0 // if you got NONE, change to 0
 
  
//---------------------------------------------------------
//internal 1.1V ref for AD
//---------------------------------------------------------

  #define INTVREF  // use internal 1.1V ref for AD


//---------------------------------------------------------
//Baudrate to Computer / other Arduino
//---------------------------------------------------------

  #define BAUDRATE 115200


//---------------------------------------------------------
//Debugging mode 
//---------------------------------------------------------

  //#define TESTRXLEVEL  // only turn on for debugging


//---------------------------------------------------------
//Tower servo
//---------------------------------------------------------

  #define standard      // 180 degree 1000us to 2000us
  //#define extended    // 270 degree 500us to 2500us


//#########################################################
// Do not touch anything under this line
//#########################################################


  #define RX
  //#define LED 13
  #define lat 0
  #define lon 1

  #define tower 13
  #define arm 12

  
  #if defined (standard)
    #define az_fact 2.777
    #define el_fact 2.777
    #define center 1500
  #endif

  #if defined (extended)
    #define el_fact 2.777
    #define az_fact 5.555
    #define center 1500
  #endif
