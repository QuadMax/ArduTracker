
#define LTMSIZE 16
uint8_t loc_chksum;

#if defined (TX)

  void sendLTM()
  {
    writePSK('$');              //  1
    writePSK('T');              //  2
    writePSK('G');              //  3
    chksum = 0;
    writePSK32(GPS_coord[0]);   //  7
    writePSK32(GPS_coord[1]);   // 11
    writePSK(GPS_speed);        // 12
    writePSK16(GPS_altitude);   // 14
    writePSK(GPS_numSat);       // 15
    loc_chksum = chksum;
    writePSK(loc_chksum);       // 16
  }
  
#endif
#if defined (RX)

  boolean receiveLTM()
  {
    if (FskAvailable() < LTMSIZE) return false;
    if (readFSK() != '$') return false;
    if (readFSK() != 'T') return false;
    if (readFSK() != 'G') return false;
    chksum = 0;
    GPS_coord[0] = readFSK32();
    GPS_coord[1] = readFSK32();
    GPS_speed = readFSK();
    GPS_altitude = readFSK16();
    GPS_numSat = readFSK();
    loc_chksum = chksum;
    return (readFSK() == loc_chksum);
  }

#endif

