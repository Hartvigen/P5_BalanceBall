float mmToPx = 0.729;
float ballRadius = 6*mmToPx;

class Table
{
  Brain brain = new PD();
  Ball ball;
  
  int boardWidth = 129, boardHeight = 119;
  int halfWidth = boardWidth/2, halfHeight = boardHeight/2;

  float degreeVelocity = timestep*0.05;
  float desiredInner = 0, desiredOuter = 0;
  float angleInner = 0, angleOuter = 0;
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
    desiredInner = 0;
    desiredOuter = 0;
    angleInner = 0; 
    angleOuter = 0;
    
    aliveTime = 0;
    
    float mod, ballx, bally;
    
    // Find starting point for ball
    if (random(1) < 0.5f)
    {
      mod = (halfWidth-ballRadius-3)/halfWidth;
      ballx = (round(random(1))*boardWidth - halfWidth) * mod;
      bally = (random(boardWidth) - halfWidth) * mod;
    }
    else
    {
      mod = (halfHeight-ballRadius-3)/halfHeight;
      ballx = (round(random(1))*boardHeight - halfHeight) * mod;
      bally = (random(boardHeight) - halfHeight) * mod;
    }

    // Find starting velocity
    float speed = (0.07 + random(0.03)) * mmToPx; // 0.729 px/mm converts from mm/ms to px/ms
    float angle = atan2(ballx, bally) + random(PI/2) - PI/4;
    if (angle < 0)
      angle += 2*PI;

    ball = new Ball(new PVector(ballx, bally), new PVector(-sin(angle)*speed, -cos(angle)*speed));
    oldX = ball.center.x;
    oldY = ball.center.y;
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
      
      float currX = ball.center.x / (runAI ? halfWidth : 1);
      float currY = ball.center.y / (runAI ? halfHeight : 1);
      
      float velX = (ball.center.x - oldX) / delayTime / 0.1*mmToPx;
      float velY = (ball.center.y - oldY) / delayTime / 0.1*mmToPx;
      
      //float xEdge = ((currX < 0 ? halfWidth : -halfWidth) + currX) / halfWidth;
      //float yEdge = ((currY < 0 ? halfHeight : -halfHeight) + currY) / halfHeight;
      
      oldX = ball.center.x;
      oldY = ball.center.y;
      
      decision = brain.compute(currX, currY, velX, velY); //, xEdge, yEdge //<>//
    } 
    else if (decisionTime < currentTime)
    {
      desiredInner = int(decision.x * (runAI ? maxAngle : 1));
      desiredOuter = int(decision.y * (runAI ? maxAngle : 1));
      
      decisionTime = 0;
    }
    updateTilt();

    ball.update(getAcceleration());
    if (abs(ball.center.x) > halfWidth || abs(ball.center.y) > halfHeight)
    {
      ball.out = true;
      return;
    }
    
    float dist = ball.center.mag();
    if(abs(desiredInner) <= 1 && abs(desiredOuter) <= 1 && dist > 25) //When the x-y degrees are stuck at zero.
    { 
      xyZeroCounter += 1;
      if(xyZeroCounter > 60*10){
        fitness = 0;
        ball.out = true;
        return;
      }
    }
    else
    {
      xyZeroCounter = 0;
    }
    
    fitness += 0.05/dist;
    if (dist < 25*mmToPx) // Convert mm to px
    {
      float value = (25*mmToPx-dist)/50*mmToPx;
      fitness += value;
    }
  }
  
  void updateTilt()
  {
    
    float xDiff = desiredInner - angleInner;
    float innerVel = abs(xDiff) <= 2 ? degreeVelocity/2 : degreeVelocity;
    
    if (abs(xDiff) <= 1) {}
    else if (abs(xDiff) > innerVel) 
    {
      if (xDiff < 0 && angleInner - innerVel >= -maxAngle)
        angleInner -= innerVel;
      else if (xDiff > 0 && angleInner + innerVel <= maxAngle)
        angleInner += innerVel;
    } 
    else
    {
      if (-maxAngle <= angleInner - xDiff && angleInner + xDiff <= maxAngle)
        angleInner += xDiff;
      else
        angleInner = maxAngle;
    }

    float yDiff = desiredOuter - angleOuter;
    float outerVel = abs(yDiff) <= 2 ? degreeVelocity/2 : degreeVelocity;
    
    if (abs(yDiff) <= 1) {}
    else if (abs(yDiff) > outerVel) 
    {
      if (yDiff < 0 && angleOuter - outerVel >= -maxAngle)
        angleOuter -= outerVel;
      else if (yDiff > 0 && angleOuter + outerVel <= maxAngle)
        angleOuter += outerVel;
    } 
    else
    {
      if (-maxAngle <= angleOuter - yDiff && angleOuter + yDiff <= maxAngle)
        angleOuter += yDiff;
      else
        angleOuter = maxAngle;
    }
  }

  PVector getAcceleration()
  {
    float cosX = cos(angleInner*0.0174532925);
    float sinX = sin(angleInner*0.0174532925);
    float sinY = sin(angleOuter*0.0174532925);
    float accMag = 5f/7f * 0.009807f;

    float xr2d = -sinX + 0f;
    float yr2d = -sinY*cosX + 0f;

    PVector acc = new PVector(xr2d, yr2d);
    acc.mult(accMag);
    acc.mult(mmToPx); // acc mm/ms^ * 0.729 px/mm = acc*0.729 px/ms^
    
    return acc;
  }
  
  
  void show(PVector pos)
  {
    // Ball and board
    noFill();
    strokeWeight(2);
    rect(pos.x-halfWidth, pos.y-halfHeight, boardWidth, boardHeight);
    strokeWeight(1);
    
    //Inner green circle
    
    if(ball.center.mag() < 25*mmToPx)
      fill(0,255,0);
    circle(pos.x, pos.y, 50*mmToPx);
    ball.show(pos);
    noFill();
    
    // Tilt info
    int offset = halfWidth+50;
    drawTiltMeter(angleInner, new PVector(pos.x + offset, pos.y));
    drawTiltMeter(-angleInner, new PVector(pos.x - offset, pos.y));
    drawTiltMeter(angleOuter, new PVector(pos.x, pos.y + offset));
    drawTiltMeter(-angleOuter, new PVector(pos.x, pos.y - offset));
  }

  void drawTiltMeter(float _ang, PVector _pos)
  {
    float magnitude = 25;

    fill(_ang > 0 ? 0 : 255, _ang > 0 ? 255 : 0, 0);
    rect(_pos.x, _pos.y, 10, magnitude * -(_ang/maxAngle));

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
