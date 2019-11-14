class Table
{
  int size, halfSize;
  PVector pos;

  float desiredX, desiredY;
  float angleX, angleY;
  float maxAngle = 15;

  float powerX, powerY;
  int z = 0;
  float avgV = 0;
  float maxV = 0;

  Ball ball = null;
  int ballRadius = 10;

  float[] decision;
  long decisionTime = 0;
  int delayTime = 150;

  float fitness = 0f;
  float allTimeBest = 0f;

  float prevX = 0f, prevY = 0f;


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
    angleX = angleY = maxAngle;

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

  boolean update() // Return true if ball is dead
  {
    if (ball != null && !ball.dead)
    {
      if (decisionTime == 0)
      {
        decisionTime = currentTime + delayTime;
      }
      
      else if (decisionTime < currentTime)
      {
        decisionTime = 0;
      }

      updateTilt();

      PVector acc = getAcceleration();
      ball.update(acc);
      if (abs(ball.center.x - pos.x) > halfSize || abs(ball.center.y - pos.y) > halfSize)
        ball.dead = true;

      return ball.dead;
    }

    return true;
  }

  void updateTilt()
  {
    float xDiff = desiredX - angleX;
    
    xDiff *= powerX;
    
    angleX += min(5, max(-5, xDiff));
    angleX = min(maxAngle, max(-maxAngle, angleX));
    float yDiff = desiredY - angleY;
    yDiff *= powerY;
    angleY += min(5, max(-5, yDiff));
    angleY = min(maxAngle, max(-maxAngle, angleY));

    //println(desiredX, desiredY, ":", angleX, angleY, ":", xDiff, yDiff);
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
