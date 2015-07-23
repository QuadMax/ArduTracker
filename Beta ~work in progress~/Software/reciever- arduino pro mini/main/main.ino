
#include <LiquidCrystal_I2C.h>   //see link above
#include "configuration.h"



//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

/*-----( Declare Variables )-----*/

int sekunde, minute, stunde, sensorValue, counter;
int crt = 0;
int K1 = 2;
int K2 = 3;
int K1read,K2read;
long TimeSetZero = 0;
long timerMenu = 0;
float voltage = 0;


#define lat 0
#define lon 1

int32_t  GPS_coord[2] = { 0, 0 };
uint16_t GPS_altitude;

void setup() {

}

void loop() {
  
  if((millis()-1000) > timerMenu){
    menu();
    timerMenu = millis();  
  }
}
