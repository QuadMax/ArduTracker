void menu()
{
//++++++++++++++++++++++++++++++++++++++++++++++++++ 
// flight time display      
//++++++++++++++++++++++++++++++++++++++++++++++++++

sekunde = ((millis()-TimeSetZero)/1000-60*crt);
  if (sekunde < 10)
  {
    if (counter == 0) {
      lcd.setCursor(13,2);  
      lcd.print((sekunde));
      lcd.setCursor(7,3);
      lcd.print(voltage+'V');
      lcd.setCursor(12,2);
      lcd.print("0");
      }
  }
  else
  {
    if (counter == 0) {
      lcd.setCursor(12,2);
      lcd.print((sekunde));
      lcd.setCursor(7,3);
      lcd.print(voltage+'V');
    }  
  }
  
  if ( sekunde > 59)
 {
   crt = crt + 1;
   minute = minute + 1;
   }
   if (minute < 10)
   {
     if (counter == 0) {
       lcd.setCursor(10,2);
       lcd.print(minute);  
     }
   }
   else
   {
     if (counter == 0) {
       lcd.setCursor(9,2);
       lcd.print(minute);
     }
   }
 
 
 if (minute > 59){
   stunde = stunde +1;
   minute = 0;
   lcd.setCursor(9,2);
   lcd.print('00');
   if ((stunde/60) < 10)
   {
     if (counter == 0) {
       lcd.setCursor(7,2);
       lcd.print((stunde/60));     
     }
   }
  else
  {
    if (counter == 0) {
      lcd.setCursor(4,1);
      lcd.print((stunde/60));
    }
 }
 } 
//++++++++++++++++++++++++++++++++++++++++++++++++++ 
// voltage reading    
//++++++++++++++++++++++++++++++++++++++++++++++++++
  
  if (counter == 0){
    sensorValue = analogRead(A2);

     voltage = sensorValue * (5.0 / 1023.0) * divider;    //220kOhm and 100kOhm voltage divider
  }
  
//++++++++++++++++++++++++++++++++++++++++++++++++++ 
// menu
//++++++++++++++++++++++++++++++++++++++++++++++++++

  // the follow variables are long's because the time, measured in miliseconds,
  // will quickly become a bigger number than can be stored in an int.
  long time = 0;         // the last time the output pin was toggled
  long debounce = 200;   // the debounce time, increase if the output flickers
  
  // if the input just went from LOW to HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  K1read = digitalRead(K1);
  
  if (K1read == HIGH && millis() - time > debounce) {
  
      TimeSetZero = millis();
      crt = 0;
      minute = 0;
      stunde = 0;
      lcd.setCursor(6,2);
      lcd.print("00:00:00");
  }
  
  K2read = digitalRead(K2);
  
  if (K2read == HIGH && millis() - time > debounce) {
      
      lcd.clear();
      lcd.print("Position:");
      lcd.setCursor(0,1);
      lcd.print(GPS_coord[lon]);
      lcd.setCursor(0,2);
      lcd.print(GPS_coord[lat]);
      lcd.setCursor(0,3);
      lcd.print(GPS_altitude);
      
      if (counter == 1){
        counter = 0;
        lcd.clear();
        lcd.setCursor(4,0);
        lcd.print("DIY-Tracker");
        lcd.setCursor(5,1);
        lcd.print("AudioArdu");
        lcd.setCursor(6,2);
        lcd.print("00:00:00");
      }
      else {
        counter = 1;
      }
  }
}
