import processing.serial.Serial; // serial library

// The serial port:
Serial myPort;   

int len = 600;

float[] GX = new float[len];

float ang;

void setup() 
{
  frameRate(10);  
  size(len, 256);
  
  // center all variables
  for (int i=0;i<len;i++) GX[i] = height/2;
  
  // List all the available serial ports:
  println(Serial.list());

  // Open the port you are using at the rate you want:  
  myPort = new Serial(this, Serial.list()[0], 115200);  
}

void draw()
{ 
  char inByte;
  float inVal;
  while (myPort.available() > 0) 
  {
    inByte = myPort.readChar();
    inVal = inByte;
    putData(inVal);
  }
  
  // Draw graphPaper
  background(255); // white
  stroke(200); // gray
  line(0, 40, width, 40);
  line(0, 256-40, width, 256-40);

  stroke(0); // black
  line(0, height/2, width, height/2); // Draw line, indicating 90 deg, 180 deg, and 270 deg

  // calculate new data
  
  drawAxisX();
}

void drawAxisX() 
{
  /* draw gyro x-axis */
  noFill();
  stroke(0,0,255); // blue
  // redraw everything
  beginShape();
  for(int i = 0; i<GX.length;i++)
    vertex(2*i,GX[i]);
  endShape();
}

int index = 0;

void putData(float in)
{
  GX[index] = in;
  index++;
  index = index%GX.length;
}



