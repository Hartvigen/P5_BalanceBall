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

  Brain brain;
  float[] decision;
  long decisionTime = 0;
  int delayTime = 150;

  float fitness = 0f;
  float allTimeBest = 0f;

  float prevX = 0f, prevY = 0f;


  Table(int _size, PVector _pos)
  {
    this(_size, _pos, new FirstBrain());
  }

  Table(int _size, PVector _pos, Brain _brain)
  {
    size = _size;
    pos = _pos;

    halfSize = size/2;
    angleX = angleY = 0;

    brain = _brain;
    reset();
  }

  Table(int _size, PVector _pos, Brain _brain, float maxFit)
  {
    size = _size;
    pos = _pos;

    halfSize = size/2;
    angleX = angleY = 0;

    brain = _brain;
    allTimeBest = maxFit;
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
    float speed = (200f + random(400)) / framerate;
    float angle = atan2(ballx - pos.x, bally - pos.y) + random(PI/2) - PI/4;
    if (angle < 0)
      angle += 2*PI;

    ball = new Ball(ballRadius, new PVector(ballx, bally), new PVector(-sin(angle)*speed, -cos(angle)*speed));
    PVector checking = ball.vel;
  }

  boolean update() // Return true if ball is dead
  {
    if (ball != null && !ball.dead)
    {
      if (decisionTime == 0)
      {
        decisionTime = currentTime + delayTime;
        float relX = ball.center.x - pos.x;
        float relY = ball.center.y - pos.y;
        println(" X = " + relX + ", Y = " + relY + ", velX = " + ball.vel.x + ", velY = " + ball.vel.y + ", D(x,e) = " + ((relX < 0 ? halfSize : -halfSize) + relX) + ", D(y,e) = " +  ((relY < 0 ? halfSize : -halfSize) + relY));  
        decision = brain.percieve(relX, relY, ball.vel.x, ball.vel.y, (relX < 0 ? halfSize : -halfSize) + relX, (relY < 0 ? halfSize : -halfSize) + relY);
      } else if (decisionTime < currentTime)
      { //<>//
        desiredX = decision[0] * maxAngle;
        desiredY = decision[1] * maxAngle;
        powerX = decision[2];
        powerY = decision[3];
        //println("X = " + desiredX + ", Y = " + desiredY + ", powerX = " + powerX + ", powerY = " + powerY);
        decisionTime = 0;
      }

      updateTilt();

      PVector acc = getAcceleration();
      ball.update(acc);
      if (abs(ball.center.x - pos.x) > halfSize || abs(ball.center.y - pos.y) > halfSize)
        ball.dead = true;

      //println(ball.vel);

      // Calculate fitness
      float dist = ball.center.dist(pos);
      if (dist == 0.0) {
        z++;
        println(z + ": " + dist);
      }
      fitness += timestep/1000f; //<>//
      fitness += min(0.5f*timestep, 5/dist);

      if (dist <= 50 && ball.vel.mag() <= 0.75f*Math.tanh(dist/20)+0.25f)
        fitness += min(0.75f, (0.1f+dist) / (0.05f+ball.vel.mag())) * timestep;

      if (ball.vel.mag() > 250)
        fitness -= ball.vel.mag() / 8;

      //if (angleX > 10 || angleY > 10)
        //fitness -= max(angleX, angleY)/frameRate;

      if (fitness < 0)
        fitness = 0;

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
      println("vel + acc = vel_a");
      print(vel.mag() + " + " + _acc.mag() +  " = ");
      vel.add(_acc);
      println(" " + vel.mag());
      
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
