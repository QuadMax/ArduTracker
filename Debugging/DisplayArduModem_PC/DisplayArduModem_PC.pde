import processing.serial.Serial; // serial library

// The serial port:
Serial myPort;   

int len = 800;
int ports;
int actport = -1;

float[] GX = new float[len];

PFont f;

void setup() 
{
  frameRate(2);  
  size(len, 256);
  
  // center all variables
  for (int i=0;i<len;i++) GX[i] = height/2;
  
  // List all the available serial ports:
  println(Serial.list());
  ports = Serial.list().length;
  
  f = createFont("Arial",16,true);
  textFont(f,16);
    
  for (int i=0;i<ports;i++) 
  {
    fill(255);
    rect(2+i*80, 2, 70, 25);
    fill(0);
    text(Serial.list()[i],10+i*80,20); 
  }
}

boolean done = false;
boolean stop = false;

void mouseClicked() 
{
  if (done)
  {
    stop = ! stop;
    return;
  }
  if ((mouseY > 2) & (mouseY < 25))
  {
    for (int i=0;i<ports;i++)
      if ((mouseX > 2+i*80) & (mouseX < 2+i*80+70)) 
      {
        actport = i;
        println(actport); 
        
        // Open the port you are using at the rate you want:  
        myPort = new Serial(this, Serial.list()[actport], 115200);
        myPort.buffer(600); 
        done = true;
      }
  }
}


void serialEvent(Serial myPort) 
{
  char inByte;
  int i;
  int no = myPort.available();
  for (i=0;i<no;i++) 
  {
    inByte = myPort.readChar();
    if (!stop) putData(inByte);
  }
}

void draw()
{ 
  float inVal;

  if (actport == -1) return;
  
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



