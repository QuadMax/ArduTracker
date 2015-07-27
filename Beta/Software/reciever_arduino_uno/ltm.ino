#define LTMSIZE 16
uint8_t loc_chksum;

#if defined (TX)

  void sendLTM()
  {
    writeFSK('$');              //  1
    writeFSK('T');              //  2
    writeFSK('G');              //  3
    chksum = 0;
    writeFSK32(GPS_coord[0]);   //  7
    writeFSK32(GPS_coord[1]);   // 11
    writeFSK(GPS_speed);        // 12
    writeFSK16(GPS_altitude);   // 14
    writeFSK(GPS_numSat);       // 15
    loc_chksum = chksum;
    writeFSK(loc_chksum);       // 16
  }
  
#endif
#if defined (RX)

  boolean receiveLTM()
  {
    if (PskAvailable() < LTMSIZE) return false;
    if (readPSK() != '$') return false;
    if (readPSK() != 'T') return false;
    if (readPSK() != 'G') return false;
    chksum = 0;
    GPS_coord[0] = readPSK32();
    GPS_coord[1] = readPSK32();
    GPS_speed = readPSK();
    GPS_altitude = readPSK16();
    GPS_numSat = readPSK();
    loc_chksum = chksum;
    return (readPSK() == loc_chksum);
  }

#endif
