
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

