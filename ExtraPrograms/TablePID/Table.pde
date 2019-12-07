class Table
{
  int size, halfSize;
  PVector pos;

  int desiredX, desiredY;
  float angleX, angleY;
  float maxAngle = 5;
  float degreeVelocity = timestep * 0.05;

  float powerX, powerY;
  int z = 0;
  float avgV = 0;
  float maxV = 0;

  Ball ball = null;
  int ballRadius = 10;

  float[] decision;
  long decisionTime = 0;
  int delayTime = 150;
  
  //values for PID
  double setPoint;
  double innerInput, outerInput;
  double innerOutput, outerOutput;
  double period = timestep;
  double integralSumInner = 0, integralSumOuter = 0;
  double lastInputInner = 0, lastInputOuter = 0;
  double outputInner = 0, outputOuter = 0;
  int sameCount = 0;
  double curX = 0, curY = 0, prevX = 0, prevY = 0;
  
  //tuning values for PID
  double IKp = 1.25, OKp = 1.25, IKi = 0, OKi = 0, IKd = 24 * timestep, OKd = 24 * timestep;

  float fitness = 0f;
  float allTimeBest = 0f;


  Table(int _size, PVector _pos)
  {
    size = _size;
    pos = _pos;

    halfSize = size/2;
    angleX = angleY = 0;

    reset();
  }

  boolean isRunning()
  {
    return ball != null && !ball.dead;
  }

  void reset()
  {
    angleX = angleY = 0;

    // Find starting point
    float mod = float(halfSize-ballRadius-3)/halfSize;
    float ballx = pos.x + (round(random(1))*size - halfSize) * mod;
    float bally = pos.y + (random(size) - halfSize) * mod;

    if (random(1) < 0.5f)
    {
      float temp = ballx;
      ballx = bally;
      bally = temp;
    }

    // Find starting velocity
    float speed = (20f + random(50)) / framerate;
    float angle = atan2(ballx - pos.x, bally - pos.y) + random(PI/2) - PI/4;
    if (angle < 0)
      angle += 2*PI;

    ball = new Ball(ballRadius, new PVector(ballx, bally), new PVector(-sin(angle)*speed, -cos(angle)*speed));
  }

  boolean update() // posReturn true if ball is dead
  {
    if (ball != null && !ball.dead)
    {
      if (decisionTime == 0)
      {
        calculatePID();
        decisionTime = currentTime + delayTime;
      }
      
      else if (decisionTime < currentTime)
      {
        decisionTime = 0;
        
          desiredX = (int)outputInner; 
          desiredY = (int)outputOuter;
      }
      
      updateTilt();

      PVector acc = getAcceleration();
      ball.update(acc);
      if (abs(ball.center.x - pos.x) > halfSize || abs(ball.center.y - pos.y) > halfSize)
        ball.dead = true;
      
      if((int)curX == (int)prevX && (int)curY == (int)prevY)
        sameCount++;
      else
        sameCount = 0;
      if(sameCount > 30)
        return true;
      
      prevX = curX;
      prevY = curY;
      
      return ball.dead;
    }

    return true;
  }

  void updateTilt()
  {
    
    float xDiff = desiredX - angleX;
    if(abs(xDiff) <= 3){}
    else if (abs(xDiff) > degreeVelocity) {
      if (xDiff < 0 && angleX - degreeVelocity >= -maxAngle) {
        angleX -= degreeVelocity;
      } else if (angleX + degreeVelocity <= maxAngle)
        angleX += degreeVelocity;
    } else
    {
      if (-maxAngle <= angleX - xDiff && angleX + xDiff <= maxAngle)
        angleX += xDiff;
      else
        angleX = maxAngle;
    }

    float yDiff = desiredY - angleY;
    if(abs(yDiff) <= 3){}
    else if (abs(yDiff) > degreeVelocity) {
      if (yDiff < 0 && angleY - degreeVelocity >= -maxAngle) {
        angleY -= degreeVelocity;
      } else if (angleY + degreeVelocity <= maxAngle)
        angleY += degreeVelocity;
    } else
    {
      if (-maxAngle <= angleY - yDiff && angleY + yDiff <= maxAngle)
        angleY += yDiff;
      else
        angleY = maxAngle;
    }
  }

  PVector getAcceleration()
  {
    float cosX = cos(angleX*0.0174532925);
    float cosY = cos(angleY*0.0174532925);
    float sinX = sin(angleX*0.0174532925);
    float sinY = sin(angleY*0.0174532925);
    float sineIncline = sin(acos(cosX * cosY));
    float accMag = 5f/7f * 9807f * sineIncline;

    float xr2d = -sinX + 0f;
    float yr2d = -sinY*cosX + 0f;

    PVector acc = new PVector(xr2d, yr2d);
    if (acc.mag() != 0)
      acc.div(acc.mag());
    acc.mult(accMag);
    acc.mult(timestep/1000f);

    return acc;
  }

  void show()
  {
    // Ball and board
    noFill();
    rect(pos.x - halfSize, pos.y - halfSize, size, size);
    if (ball != null)
      ball.show();

    // Tilt info
    int offset = halfSize + 20;
    drawTiltMeter(angleX, new PVector(pos.x + offset, pos.y));
    drawTiltMeter(-angleX, new PVector(pos.x - offset, pos.y));
    drawTiltMeter(angleY, new PVector(pos.x, pos.y + offset));
    drawTiltMeter(-angleY, new PVector(pos.x, pos.y - offset));
  }

  void drawTiltMeter(float _ang, PVector _pos)
  {
    float height = -_ang / 2;

    fill(_ang > 0 ? 0 : 255, _ang > 0 ? 255 : 0, 0);
    rect(_pos.x, _pos.y, 10, height);

    fill(0);
    rect(_pos.x, _pos.y-1, 10, 2);
    if (_pos.x > 300)
      text(_ang, _pos.x + 15, _pos.y);
    else
      text(_ang, _pos.x- 50, _pos.y);
  }
  
  void calculatePID(){
    curX = ball.center.x - pos.x;
    curY = ball.center.y - pos.y;
    
    double proportionalInner = IKp * curX;
    double integralInner     = integralSumInner + IKi * curX * period;
    double derivativeInner   = IKd * (curX - lastInputInner) / period;

    double proportionalOuter = OKp * curY;
    double integralOuter     = integralSumOuter + OKi * curY * period;
    double derivativeOuter   = OKd * (curY - lastInputOuter) / period;

    integralSumInner = integralInner;
    integralSumOuter = integralOuter;

    lastInputInner = curX;
    lastInputOuter = curY;

    outputInner = proportionalInner + integralInner + derivativeInner;
    outputOuter = proportionalOuter + integralOuter + derivativeOuter;
      
    
  }
}


class Ball
{
  boolean dead = false;

  int radius;
  PVector center;
  PVector vel;

  Ball(int _radius, PVector _center, PVector _vel)
  {
    radius = _radius;
    center = _center;
    vel = _vel;
  }

  void update(PVector _acc)
  {
    if (!dead)
    {
      vel.add(_acc);
      //vel.mult(vel.mag() > 0.1 ? 0.997 : 0);
      center.add(vel.copy().mult(timestep/1000f));
    }
  }

  void show()
  {
    fill(0, 0, 0);
    circle(center.x, center.y, radius);
  }
  
}
 
