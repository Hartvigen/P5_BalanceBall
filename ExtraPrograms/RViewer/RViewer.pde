import processing.serial.*;

Serial serial;

int time;

int boardWidth, boardHeight, maxAngle;

int reportedTime, actualTime;
boolean ballFound;
int ballX, ballY;
int innerAng, outerAng;
int desiredInner, desiredOuter;

void setup() 
{
  size(350, 350);
  textFont(createFont("Consolas",10));
  frameRate(60);
  smooth();
  
  printArray(Serial.list());
  println("Creating connection to: '" + Serial.list()[1] + "'...");
  serial = new Serial(this, Serial.list()[1], 115200*6);
  println("Connected!");
  serial.clear();
  
  delay(2000);
  
  boardHeight = readInt();
  boardWidth = readInt();
  maxAngle = readInt();
  
  println("Ready!");
}

void draw()
{
  reportedTime = readInt();
  ballFound = readInt() != 0;
  ballX = readInt();
  ballY = readInt();
  innerAng = readInt();
  outerAng = readInt();
  desiredInner = readInt();
  desiredOuter = readInt();
  
  actualTime = millis() - time;
  time = millis();
  
  drawTable();
  println(millis());
}

void drawTable()
{
  background(255, 204, 0);
  
  noFill();
  rect(width/2-boardWidth/2, height/2-boardHeight/2, boardWidth, boardHeight);
  
  if (ballFound)
  {
    fill(0);
    circle(width/2+ballX, height/2+ballY, 3);
    noFill();
  }
  
  int offsetX = boardWidth/2+25;
  int offsetY = boardHeight/2+25;
  drawTiltMeter(innerAng, desiredInner, new PVector(width/2 + offsetX, height/2));
  drawTiltMeter(-innerAng, -desiredInner, new PVector(width/2 - offsetX, height/2));
  drawTiltMeter(outerAng, desiredOuter, new PVector(width/2, height/2 + offsetY));
  drawTiltMeter(-outerAng, -desiredOuter, new PVector(width/2, height/2 - offsetY));
  
  text("Delay: " + reportedTime + " (" + actualTime + ")", 5, 15);
}

void drawTiltMeter(float _ang, float _desire, PVector _pos)
{
  float magnitude = 10;

  fill(_ang > 0 ? 0 : 255, _ang > 0 ? 255 : 0, 0);
  rect(_pos.x, _pos.y, 10, magnitude * (_ang/maxAngle));

  fill(0);
  rect(_pos.x, _pos.y-1, 10, 2);
  
  if (_pos.x > width/2)
    text(_ang + "\n(" + _desire + ")", _pos.x + 15, _pos.y);
  else
    text(_ang + "\n(" + _desire + ")", _pos.x - 50, _pos.y);
}


// ================================================================================================ //


int readInt()
{
  while (serial.available() < 4) { delay(2); }
  
  byte[] bytes = serial.readBytes(4);
  int value = int(bytes[0]) | int(bytes[1]) << 8 | int(bytes[2]) << 16 | int(bytes[3]) << 24;
  
  return value;
}
