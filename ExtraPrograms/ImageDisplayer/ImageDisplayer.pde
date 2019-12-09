import processing.serial.*;

Serial serial;

void setup() 
{
  size(400, 400);
  colorMode(RGB, 31, 63, 31);
  noStroke(); // Don't draw a stroke around shapes
  
  printArray(Serial.list());
  println("Creating connection to: '" + Serial.list()[0] + "'...");
  serial = new Serial(this, Serial.list()[0], 115200 * 6);
  println("Connected!");
  serial.clear();
  
  delay(2000);
  
  readInt();
  readInt();
  readInt();
  println("Ready!");
}

void draw()
{
  
}

void keyPressed()
{
  println("reading");
  serial.write(0x00);
  
  int h = readInt();
  int w = readInt();
  
  println("Size (" + w + "," + h + ")");
  
  ArrayList<byte[]> image = new ArrayList<byte[]>();
  
  for (int i = 0; i < h; i++)
  {
    serial.write(0x00);
    image.add(readBytes());
  }
  
  serial.write(0x00);
  int xC = readInt();
  serial.write(0x00);
  int yC = readInt();
  
  println(xC, yC);
  
  background(0,0,0);
  
  for (int y = 0; y < h; y++)
  {
    byte[] bytes = image.get(y);
    for (int x = 0; x < w; x++)
    {
      fill(bytes[x*3], bytes[x*3+1], bytes[x*3+2]);
      rect(x, y, 1, 1);
    }
  }
  
  fill(0,63,0);
  rect(xC + w/2, yC + h/2, 1, 1);
  
  println("done");
}

byte[] readBytes() 
{
  int size = readInt();
  
  while (serial.available() < size) { delay(2); }
  byte[] bytes = serial.readBytes(size);
  
  return bytes;
}

int readInt()
{
  while (serial.available() < 4) { delay(2); }
  
  byte[] bytes = serial.readBytes(4);
  int value = int(bytes[0]) | int(bytes[1]) << 8 | int(bytes[2]) << 16 | int(bytes[3]) << 24;
  
  return value;
}
