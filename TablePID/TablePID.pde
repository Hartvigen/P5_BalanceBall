int framerate = 60; //<>// //<>// //<>// //<>// //<>//
long currentTime = 0;
int timestep = 1000/framerate;
int infoBoxHeight = 65;

Table[] tables;


int epoch = 1;
int genSize = 20;
int stillRunning = genSize;
int triesPerGen = 1000;
int tryNr = 1;

//values for PID
double setPoint;
double innerInput, outerInput;
double innerOutput, outerOutput;
int period = 5;
double integralSumInnner = 0, integralSumOuter = 0;
float lastInputInner = 0, lastInputOuter = 0;
double outputInner = 0, outputeOuter = 0;

//tuning values for PID
double IKp = 0.25, Okp = 0.25, IKi = 0, OKi = 0, IKd = 0, OKd = 0;

int showOption = 0;
int showSpecificIndex = 0;
int shownIndex = -1;

float maxFitness = 0f;

void setup()
{
  size(500, 500);

  frameRate(framerate);
  smooth();

  textFont(createFont("Consolas", 10));

  initialize();
}

void initialize()
{
  tables = new Table[genSize];
  for (int i = 0; i < genSize; i++) 
      tables[i] = new Table(250, new PVector(width/2, height/2));
}

void draw()
{
  currentTime += timestep;
  background(255, 204, 0);

  for (int i = 0; i < genSize; i++)
  {
    if (tables[i].isRunning())
      if (tables[i].update())
        stillRunning--;
  }

  if (stillRunning > 0)
    drawTable();
  else if (tryNr < triesPerGen)
  {
    tryNr++;
    for (int i = 0; i < genSize; i++)
      tables[i].reset();
    stillRunning = genSize;
  }

  drawInfo();
}

void drawTable()
{
  if (showOption == 0) // First still running
  {
    int i;
    for (i = 0; i < genSize; i++)
      if (tables[i].isRunning())
      {
        tables[i].show();
        shownIndex = i;
        break;
      }

    if (i == genSize)
      shownIndex = -1;
  } else if (showOption == 1) // Highest fitness
  {
    float max = tables[0].fitness;
    for (int i = 1; i < genSize; i++)
      if (tables[i].fitness > max)
      {
        shownIndex = i;
        max = tables[shownIndex].fitness;
      }

    tables[shownIndex].show();
  } else if (showOption == 2) // Show specific
  {
  }
}

void drawInfo()
{
  fill(200);
  rect(-1, -1, width+1, infoBoxHeight);

  fill(0);
  text("FPS:     " + (int)frameRate, 5, 10);
  text("Running: " + stillRunning + "/" + genSize, 5, 20);
  text("Showing: " + shownIndex + "(" + (shownIndex != -1 ? tables[shownIndex].fitness : 0f) + ")", 5, 30);
  text("Epoch:   " + epoch, 5, 40);
  text("Max fit: " + maxFitness, 5, 50);
  text("Try nr.: " + tryNr + "/" + triesPerGen, 5, 60);
}

void mousePressed()
{
}
