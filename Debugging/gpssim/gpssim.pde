import processing.serial.*;

// The serial port:
Serial myPort;   

int hours = 0;
int mins = 0;
int secs = 55;
int msecs = 0;

int b_deg = 0; // 45704
int b_min = 1; 
int b_p_min = 0; // 5000 = 927 los
int l_deg = 0;
int l_min = 1;
int l_p_min = 0;

int cent_v = 200;
int cent_h = 250;

int gpsrate = 5;

void setup () 
{ 
  frameRate(gpsrate);  
  
  // Sketch einstellen
  size (500, 400);
  stroke (255);

  // List all the available serial ports:
  println(Serial.list());

  // Open the port you are using at the rate you want:
  myPort = new Serial(this, Serial.list()[1], 57600);
}
 
int satct = 0;

int b = 0;
int l = 0;

float speed = 1.0;
float actdir = 0.0;

int g_b,g_l;
int old_g_b = 200;
int old_g_l = 250;

void mousePressed()
{
  int dx,dy;
  float dir;
  dx = mouseY - g_b;
  dy = mouseX - g_l;
  dir = atan2(dy,-dx) * 57.29;
  actdir = dir;
  speed = 20.0;
  
  println(dir);
}


void draw () 
{
  String stri;
  String checksum;
  byte res = 0;
  float dirrad;
 
  //actdir += 2;
  dirrad = actdir / 57.29;
  b += (int)(cos(dirrad) * speed);
  l += (int)(sin(dirrad) * speed);
  
  b_p_min = 5000 + (b % 100000);
  l_p_min = 5000 + (l % 100000);
  
  g_b = 200 - b/10;
  g_l = 250 + l/10;
  
  line(g_l,g_b,old_g_l,old_g_b);
  old_g_b = g_b;
  old_g_l = g_l;
  
  //stri = "$GPGGA,202805.00,4723.20958,N,00837.26604,E,1,10,0.87,455.0,M,47.4,M,,*";
  stri = "$GPGGA," + 
         nf(hours,2) + nf(mins,2) + nf(secs,2) + '.' + nf(msecs,2) + ',' + 
         nf(b_deg,2) + nf(b_min,2) + '.' + nf(b_p_min,5) +
         ",N," +
         nf(l_deg,3) + nf(l_min,2) + '.' + nf(l_p_min,5) +
         ",E," +
         "1," + satct + ",0.87,500.0,M,47.4,M,,*";
        
  res = 0;
  for(int i=1; i<stri.length()-1; i++) res ^= byte(stri.charAt(i));
  checksum = hex(res);
  stri += checksum;

  println(stri);
  myPort.write(stri);
  myPort.write(13);
  myPort.write(10);

  if (satct < 8) satct++;

/*  
//  //stri = "$GPRMC,202805.00,A,4723.20958,N,00837.26604,E,0.027,,110213,,,A*";
  stri = "$GPRMC," + 
         nf(hours,2) + nf(mins,2) + nf(secs,2) +".00," +
         "A," + 
         nf(b_deg,2) + nf(b_min,2) + '.' + nf(b_p_min,5) +
         ",N," +
         nf(l_deg,3) + nf(l_min,2) + '.' + nf(l_p_min,4) +
         ",E," +
         "0.0,178,110213,,,A*";

  res = 0;
  for(int i=1; i<stri.length()-1; i++) res ^= byte(stri.charAt(i));
  checksum = hex(res);
  stri += checksum;

  //println(stri);
  //myPort.write(stri);
  //myPort.write(13);
  //myPort.write(10);
*/

  msecs += 100 / gpsrate;
  if (msecs >= 100) { msecs -= 100; secs++; }
  if (secs >= 60) { secs -= 60; mins++; }
  if (mins >= 60) { mins -= 60; hours++; }
  
}
