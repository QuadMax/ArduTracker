void init_compass(){
  compass = HMC5883L(); // Construct a new HMC5883 compass.
  error = compass.SetScale(1.3); // Set the scale of the compass.
  error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous

}

void get_heading(){

  MagnetometerScaled scaled = compass.ReadScaledAxis();

  // Values are accessed like so:
  int MilliGauss_OnThe_XAxis = scaled.XAxis;// (or YAxis, or ZAxis)

  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(scaled.YAxis, scaled.XAxis);
  
  heading += declinationAngle;
  heading = heading * (-1) + 2*PI;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  
  headingDegrees   = (uint16_t)(heading * 180/PI); 

}

void send_heading(){
  byte LByte =  headingDegrees;
  byte HByte =  (headingDegrees >> 8);

  byte test = (B11111111);

  SerialWrite(test);
  SerialWrite(LByte);
  SerialWrite(HByte);
}

void stop_compass(){
  
  compass.SetMeasurementMode(Measurement_Idle);
  
}



