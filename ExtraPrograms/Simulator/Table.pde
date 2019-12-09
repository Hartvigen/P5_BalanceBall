int ballRadius = 6;

class Table
{
  Brain brain = new PD();
  Ball ball;
  
  int size = 210;
  int halfSize = size/2;

  float degreeVelocity = timestep*0.05;
  float desiredX = 0, desiredY = 0;
  float angleX = 0, angleY = 0;
  float maxAngle = 5;
  
  PVector decision;
  long decisionTime = 0;
  int delayTime = 184;
  int aliveTime = 0;
  
  float fitness = 0f;
  float allTimeBestFit = 0f;

  int xyZeroCounter = 0;
  
  float oldX, oldY;


  Table()
  {
    brain = new AI();
    
    reset();
  }

  Table(Brain _brain)
  {
    brain = _brain;
    
    reset();
  }

  Table(Brain _brain, float maxFit)
  {
    brain = _brain;
    allTimeBestFit = maxFit;
    
    reset();
  }
  
  
  public boolean isRunning()
  {
    return !ball.out;
  }
  
  
  public void reset()
  {
    desiredX = 0;
    desiredY = 0;
    angleX = 0; 
    angleY = 0;
    
    aliveTime = 0;
    
    // Find starting point for ball
    float mod = float(halfSize-ballRadius-3)/halfSize;
    float ballx = (round(random(1))*size - halfSize) * mod;
    float bally = (random(size) - halfSize) * mod;

    if (random(1) < 0.5f)
    {
      float temp = ballx;
      ballx = bally;
      bally = temp;
    }

    // Find starting velocity
    float speed = (0.07 + random(0.03));
    float angle = atan2(ballx, bally) + random(PI/2) - PI/4;
    if (angle < 0)
      angle += 2*PI;

    ball = new Ball(new PVector(ballx, bally), new PVector(-sin(angle)*speed, -cos(angle)*speed));
  }
  
  
  public void update()
  {
    aliveTime += timestep;
    if (aliveTime > 60000) {
      ball.out = true;
      return;
    }
    
    if (decisionTime == 0)
    {
      decisionTime = currentTime + delayTime;
      
      float currX = ball.center.x;
      float currY = ball.center.y;
      
      float velX = (currX - oldX) / delayTime;
      float velY = (currY - oldY) / delayTime;
      
      oldX = currX;
      oldY = currY;
      
      decision = brain.compute(currX, currY, velX, velY, (currX < 0 ? halfSize : -halfSize) + currX, (currY < 0 ? halfSize : -halfSize) + currY);
    } 
    else if (decisionTime < currentTime)
    {
      desiredX = int(decision.x * (runAI ? maxAngle : 1));
      desiredY = int(decision.y * (runAI ? maxAngle : 1));
      
      decisionTime = 0;
    }
    updateTilt();
    
    float dist = ball.center.mag();
    if(abs(desiredX) <= 1 && abs(desiredY) <= 1 && dist > 25) //When the x-y degrees are stuck at zero.
    { 
      xyZeroCounter += 1;
      if(xyZeroCounter > 60*5){
        fitness = 0;
        ball.out = true;
        return;
      }
    }
    else
    {
      xyZeroCounter = 0;
    }

    ball.update(getAcceleration());
    if (abs(ball.center.x) > halfSize || abs(ball.center.y) > halfSize)
    {
      ball.out = true;
      return;
    }
    
    if (dist < 25)
    {
      float value = (25-dist)/100;
      fitness += value;
    }
  }
  
  void updateTilt()
  {
    float xDiff = desiredX - angleX;
    if (abs(xDiff) <= 1) {}
    else if (abs(xDiff) > degreeVelocity) 
    {
      if (xDiff < 0 && angleX - degreeVelocity >= -maxAngle)
        angleX -= degreeVelocity;
      else if (xDiff > 0 && angleX + degreeVelocity <= maxAngle)
        angleX += degreeVelocity;
    } 
    else
    {
      if (-maxAngle <= angleX - xDiff && angleX + xDiff <= maxAngle)
        angleX += xDiff;
      else
        angleX = maxAngle;
    }

    float yDiff = desiredY - angleY;
    if (abs(yDiff) <= 1) {}
    else if (abs(yDiff) > degreeVelocity) 
    {
      if (yDiff < 0 && angleY - degreeVelocity >= -maxAngle)
        angleY -= degreeVelocity;
      else if (yDiff > 0 && angleY + degreeVelocity <= maxAngle)
        angleY += degreeVelocity;
    } 
    else
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
    float sinX = sin(angleX*0.0174532925);
    float sinY = sin(angleY*0.0174532925);
    float accMag = 5f/7f * 0.009807f;

    float xr2d = -sinX + 0f;
    float yr2d = -sinY*cosX + 0f;

    PVector acc = new PVector(xr2d, yr2d);
    acc.mult(accMag);
    
    return acc;
  }
  
  
  void show(PVector pos)
  {
    // Ball and board
    noFill();
    strokeWeight(2);
    rect(pos.x-halfSize, pos.y-halfSize, size, size);
    strokeWeight(1);
    
    //Inner green circle
    
    if(ball.center.mag() < 25)
      fill(0,255,0);
    circle(pos.x, pos.y, 50);
    ball.show(pos);
    noFill();
    
    // Tilt info
    int offset = halfSize+50;
    drawTiltMeter(angleX, new PVector(pos.x + offset, pos.y));
    drawTiltMeter(-angleX, new PVector(pos.x - offset, pos.y));
    drawTiltMeter(angleY, new PVector(pos.x, pos.y + offset));
    drawTiltMeter(-angleY, new PVector(pos.x, pos.y - offset));
  }

  void drawTiltMeter(float _ang, PVector _pos)
  {
    float magnitude = 25;

    fill(_ang > 0 ? 0 : 255, _ang > 0 ? 255 : 0, 0);
    rect(_pos.x, _pos.y, 10, magnitude * (_ang/maxAngle));

    fill(0);
    rect(_pos.x, _pos.y-1, 10, 2);
    
    if (_pos.x > 300)
      text(_ang, _pos.x + 15, _pos.y);
    else
      text(_ang, _pos.x - 50, _pos.y);
  }
}


class Ball
{
  boolean out = false;
  
  PVector center;
  PVector vel;

  Ball(PVector _center, PVector _vel)
  {
    center = _center;
    vel = _vel;
  }

  void update(PVector _acc)
  {
    if (!out)
    {
      _acc.mult(timestep);
      vel.add(_acc);
      center.add(vel.copy().mult(timestep));
    }
  }

  void show(PVector offset)
  {
    fill(0);
    circle(center.x + offset.x, center.y + offset.y, ballRadius);
  }
}
