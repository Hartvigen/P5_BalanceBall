import processing.serial.*;

Serial udSerial;

IntList image;
int length = -1;
int row = 0, col = 0;

int imageWidth = 320;
int imageHeight = 240;

boolean testrun = true;
float Rmin = 31, Gmin = 63, Bmin = 31;

class Packet {
  int code;
  byte[] data;
}


void setup() {
  size(320, 240);
  colorMode(RGB, 31,63,31);
  noStroke();  // Don't draw a stroke around shapes
  
  printArray(Serial.list());
  println("Creating connection to: '" + Serial.list()[0] + "'...");
  udSerial = new Serial(this, Serial.list()[0], 115200*6);
  println("Connected!");
  udSerial.clear();
  delay(2000);
}

void draw() {
  
}

void keyPressed(){
  row = col = 0;
  fill(0);
  rect(0,0,width,height);
  float R, G, B;
  float[][][] image = new float[imageHeight][imageWidth][3];
  float X_co = 0, Y_co = 0, P_c = 0;
  
  udSerial.write(1);
  println("Getting image... ");
  for (int i = 0; i < imageHeight; i++)
  {
    Packet pac = readSerial();
    for (int x = imageWidth-2; x >= 0; x-- ){
      
      R = (float) pac.data[x*3];
      G = (float) pac.data[x*3+1];
      B = (float) pac.data[x*3+2];
      image[i][x][0] = R;
      image[i][x][1] = G;
      image[i][x][2] = B;
      if(testrun){
        if(Rmin > R)
          Rmin = R;
        if(Gmin > G)
          Gmin = G;
        if(Bmin > B)
          Bmin = B;
      }
      else if(i % 5 == 0 && R < Rmin && B < Bmin && G < Gmin){
        image[i][x][0] = 31;
        P_c = P_c + 1;
        X_co = (x - X_co)/P_c + X_co;
        Y_co = (i - Y_co)/P_c + Y_co;        
      }
        
      /*else{
        R = 0;
        G = 0;
        B = 0;
      }*/
    }
  
    udSerial.write(1);
  }
  for (int h = 0; h < imageHeight; h++)
      for (int x = imageWidth-2; x >= 0; x-- ){
        if(x == (int) X_co && h == (int) Y_co)
          drawPixel(0, 63, 0);
        drawPixel((int) image[h][x][0], (int) image[h][x][1], (int) image[h][x][2]);
      }
  println("X = " + X_co);
  println("Y = " + Y_co);
  println("Done!");
  if(testrun)
    testrun = false;
}


Packet readSerial()
{
  Packet pac = new Packet();
  
  while (udSerial.available() < 5) { delay(2); }
  byte[] header = udSerial.readBytes(5);
  
  pac.code = int(header[0]);
  int size = int(header[1]) | int(header[2]) << 8 | int(header[3]) << 16 | int(header[4]) << 24;
  //println("Getting:", pac.code, size, int(header[1]), int(header[2]), int(header[3]), int(header[4]));
  
  while (udSerial.available() < size) { delay(2); }
  pac.data = udSerial.readBytes(size);
  
  return pac;
}

void writeSerial(int code, byte[] data)
{
  int size = data.length;
  byte[] header = new byte[] { byte(code), byte(size), byte(size >> 8), byte(size >> 16), byte(size >> 24) };
  //println("Sending: " + code + ",", size, size >> 8, size >> 16, size >> 24);
  
  udSerial.write(concat(header, data));
}




int unsign(byte b)
{
  return b < 0 ? 256 + b : b;
}

void drawPixel(int r, int g, int b)
{
  fill(r,g,b);
  rect(col, row, 1, 1);
  
  if ((col = ((col + 1) % (imageWidth -1))) == 0)
    row++;
}



void method2()
{
  if (udSerial.available() > 0)
  {
    if (image == null)
    {
      if (udSerial.available() >= 4)
      {
        byte[] lb = udSerial.readBytes(4);
        length = int(lb[0]) | int(lb[1]) << 8 | int(lb[2]) << 16 | int(lb[3]) << 24;
        
        if (length > 0 && length <= 300000)
        {
          println("New image with length: " + length);
          image = new IntList(length);
          udSerial.write(1);
        }
        else
          println("Bad length: " + length);
      }
    }
    else
    {
      for (byte b : udSerial.readBytes())
        image.append(b < 0 ? 256 + b : b);
    }
  }
      
  if (image != null && image.size() >= length)
  {
    int pixs = length/2;
    int[] bytes = image.array();
    for (int x = 0; x < pixs; x++)
    {
      int c565 = bytes[x*2] | bytes[x*2+1] << 8;
      
      int r = ((((c565 >> 11) & 0x1F) * 527) + 23) >> 6;
      int g = ((((c565 >> 5) & 0x3F) * 259) + 33) >> 6;
      int b = (((c565 & 0x1F) * 527) + 23) >> 6;
      
      drawPixel(r,g,b);
    }
    
    image = null;
    println("Print!");
  }
}

void method1()
{
  int length = 2*width;
  if (udSerial.available() >= length) 
  {
    byte[] bytes = udSerial.readBytes(length);
    int pixs = bytes.length / 2;
    println("Pixs: " + pixs);
    
    for (int x = 0; x < pixs; x++)
    {
      byte b1 = bytes[x*2];
      byte b2 = bytes[x*2+1];
      
      int ib1 = b1 < 0 ? 256 + b1 : b1;
      int ib2 = b2 < 0 ? 256 + b2 : b2;
      
      int c565 = ib1 | ib2 << 8;
      
      /*
      int r = b1 < 0 ? 256 + b1 : b1;
      int g = b2 < 0 ? 256 + b2 : b2;
      int b = b3 < 0 ? 256 + b3 : b3;
      */
      
      int r = ((((c565 >> 11) & 0x1F) * 527) + 23) >> 6;
      int g = ((((c565 >> 5) & 0x3F) * 259) + 33) >> 6;
      int b = (((c565 & 0x1F) * 527) + 23) >> 6;
      
      drawPixel(r,g,b);
    }
  }
}
